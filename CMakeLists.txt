cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(r-type_client LANGUAGES CXX)

set(GRINGO_REPOSITORY_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(GRINGO_ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Gringo Engine")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# R-Type is the only project built by this entry point.
# ============================================================

# This entry point builds only the mandatory R-Type client and server.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GRINGO_REPOSITORY_DIR}$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GRINGO_REPOSITORY_DIR}$<0:>)
set(RTYPE_PLUGIN_DIRECTORY "${GRINGO_ENGINE_DIR}/plugins$<0:>")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH "\$ORIGIN;\$ORIGIN/Gringo Engine")

# ============================================================
# Dependencies
# ============================================================

set(CPM_SOURCE_CACHE
    ${GRINGO_REPOSITORY_DIR}/.cpm-cache
    CACHE PATH "CPM download cache"
)

include("${GRINGO_REPOSITORY_DIR}/CPM.cmake")

# raylib is needed by the R-Type client renderer plugin.
CPMAddPackage(
    NAME raylib
    GITHUB_REPOSITORY raysan5/raylib
    GIT_TAG 6.0
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_SHARED_LIBS OFF"
)

if(TARGET raylib)
    target_compile_options(raylib PRIVATE -w)
endif()

find_package(Threads REQUIRED)

CPMAddPackage(
    NAME asio
    VERSION 1.30.2
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-30-2
    DOWNLOAD_ONLY YES
)

if(asio_ADDED)
    add_library(asio INTERFACE)
    target_include_directories(asio SYSTEM INTERFACE "${asio_SOURCE_DIR}/asio/include")
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
    target_link_libraries(asio INTERFACE Threads::Threads)
    if(WIN32)
        target_link_libraries(asio INTERFACE ws2_32 wsock32)
        target_compile_definitions(asio INTERFACE _WIN32_WINNT=0x0A00)
    endif()
endif()

CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
    OPTIONS "JSON_BuildTests OFF"
)

# ============================================================
# Engine (shared library used by the executables and every plugin)
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rtype_engine SHARED
    ${GRINGO_ENGINE_DIR}/src/ECS/app/Application.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/app/PluginLoader.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/ComponentManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/Scene.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/TickTimeManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/EntityManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/SystemManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/ScriptSystem.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/Prefab.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(rtype_engine PUBLIC
    ${GRINGO_ENGINE_DIR}/src/ECS/app
    ${GRINGO_ENGINE_DIR}/src/ECS/core
    ${GRINGO_ENGINE_DIR}/src/ECS/errors
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs
    ${GRINGO_ENGINE_DIR}/src/ECS/systems
    ${GRINGO_ENGINE_DIR}/src/ECS/components
    ${GRINGO_ENGINE_DIR}/src/ECS/input
    ${GRINGO_ENGINE_DIR}/src/ECS/resources
)

target_link_libraries(rtype_engine PUBLIC nlohmann_json::nlohmann_json PRIVATE ${CMAKE_DL_LIBS})

# Asio and PluginLoader pull in windows.h, whose GDI/USER declarations (Rectangle, CloseWindow,
# LoadImage, SendInput...) clash with raylib and our own names. PUBLIC: every target links the engine
target_compile_definitions(rtype_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)

# MSVC exports nothing from a DLL by default: export every engine function for the plugins
set_target_properties(rtype_engine PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
)

# ============================================================
# Plugins (one shared library each, loaded at runtime from plugins/)
# ============================================================

# add_plugin(<target> <file name> <sources...>) builds plugins/<file name>.so (.dll on Windows)
function(add_plugin target name)
    add_library(${target} SHARED ${ARGN})
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME ${name}
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY ${RTYPE_PLUGIN_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY ${RTYPE_PLUGIN_DIRECTORY}
        INSTALL_RPATH "\$ORIGIN;\$ORIGIN/.."
    )
    target_link_libraries(${target} PUBLIC rtype_engine)
endfunction()

# Renderer and Physics are built for both the game and the editor
add_plugin(plugin_renderer RayLibRenderer
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/RayLibRenderer.cpp
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/systems/RenderSystem.cpp
)

target_include_directories(plugin_renderer PUBLIC
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/components
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/resources
    ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/systems
)

target_include_directories(plugin_renderer SYSTEM PUBLIC
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(plugin_renderer PRIVATE raylib)

add_plugin(plugin_physics Physics
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics/Physics.cpp
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics/systems/Collider2DSystem.cpp
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics/systems/MovementSystem.cpp
)

target_include_directories(plugin_physics PUBLIC
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics/components
    ${GRINGO_ENGINE_DIR}/src/plugins/Physics/systems
)

set(RTYPE_TARGETS
    rtype_engine
    plugin_renderer
    plugin_physics
)

# ============================================================
# R-Type network plugin, game plugins, client and server
# ============================================================

# The R-Type plugins link the Network plugin directly: they call NetTransport and Packet
    add_plugin(plugin_network Network
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/NetworkPlugin.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/resources/NetTransport.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/Connection.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/Packet.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/resources/UdpSocket.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/systems/NetReceiveSystem.cpp
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/systems/NetSendSystem.cpp
    )

    target_include_directories(plugin_network PUBLIC
        ${GRINGO_ENGINE_DIR}/src/plugins/Network
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/resources
        ${GRINGO_ENGINE_DIR}/src/plugins/Network/systems
    )

    target_link_libraries(plugin_network PUBLIC asio)

    # Exports NetTransport, Packet... for the R-Type plugins (the others only export createPlugin)
    set_target_properties(plugin_network PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

    add_plugin(plugin_rtype_client RTypeClient
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/RTypeClientPlugin.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems/ClientNetSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems/SendInputSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_client PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/components
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
    )

    # Only the Renderer and Physics headers (components, resources) are used, not their code
    target_include_directories(plugin_rtype_client PRIVATE
        $<TARGET_PROPERTY:plugin_renderer,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_client PRIVATE plugin_network)

    add_plugin(plugin_rtype_server RTypeServer
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/RTypeServerPlugin.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/ServerNetSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/PlayerInputSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/SnapshotSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_server PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/components
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
        $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_server PRIVATE plugin_network)

    # ---- Client ----

    add_executable(${PROJECT_NAME}
        ${GRINGO_REPOSITORY_DIR}/R-Type/main.cpp
        ${GRINGO_ENGINE_DIR}/src/runtime/ProjectComponentManager.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GRINGO_ENGINE_DIR}/src/runtime
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
        ${GRINGO_ENGINE_DIR}/src/plugins/Network
        ${GRINGO_ENGINE_DIR}/src/plugins/Physics/components
        ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/components
        ${GRINGO_ENGINE_DIR}/src/plugins/Renderer/resources
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GRINGO_USE_PHYSICS
        GRINGO_USE_RENDERER
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        rtype_engine
        asio
        nlohmann_json::nlohmann_json
    )

    # The executables only load the plugins at runtime: build them too, so they are next to the binaries
    add_dependencies(${PROJECT_NAME} plugin_renderer plugin_physics plugin_network plugin_rtype_client)

    # ---- Server ----

    add_executable(r-type_server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/main.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/ServerLoop.cpp
    )

    target_include_directories(r-type_server PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_ENGINE_DIR}/src/plugins/Network
    )

    target_link_libraries(r-type_server PRIVATE rtype_engine asio)

    add_dependencies(r-type_server plugin_physics plugin_network plugin_rtype_server)

    list(APPEND RTYPE_TARGETS
        plugin_network
        plugin_rtype_client
        plugin_rtype_server
        ${PROJECT_NAME}
        r-type_server
    )
# ============================================================
# Compiler warnings
# ============================================================

foreach(target IN LISTS RTYPE_TARGETS)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif()
endforeach()

# ============================================================
# clang-tidy (runs on every compile of the engine, plugins, client and server)
# ============================================================

option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" ON)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
        set_target_properties(${RTYPE_TARGETS} PROPERTIES
            CXX_CLANG_TIDY
                "${CLANG_TIDY_EXE};--config-file=${GRINGO_REPOSITORY_DIR}/Norms/.clang-tidy"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()