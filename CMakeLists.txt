cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# What to build
# ============================================================

# cmake -B build -DBUILD_GRINGO=ON -DBUILD_RTYPE=OFF   -> ./Gringo only
# cmake -B build -DBUILD_GRINGO=OFF -DBUILD_RTYPE=ON    -> r-type_client and r-type_server only
# cmake -B build -DBUILD_GRINGO=ON -DBUILD_RTYPE=ON     -> everything

option(BUILD_GRINGO "Build the ./Gringo editor" OFF)
option(BUILD_RTYPE "Build r-type_client and r-type_server" ON)

if(NOT BUILD_GRINGO AND NOT BUILD_RTYPE)
    message(FATAL_ERROR "Nothing to build: enable BUILD_GRINGO and/or BUILD_RTYPE")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}$<0:>)
set(RTYPE_PLUGIN_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins$<0:>)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")

# ============================================================
# Dependencies
# ============================================================

set(CPM_SOURCE_CACHE
    ${CMAKE_SOURCE_DIR}/.cpm-cache
    CACHE PATH "CPM download cache"
)

include(CPM.cmake)

# raylib is needed by the Renderer plugin, which both the game and the editor use
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

if(BUILD_RTYPE)
    # Standalone Asio is header-only and has no CMakeLists, so we wrap it in an
    # interface target
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
endif()

if(BUILD_GRINGO)
    CPMAddPackage(
        NAME nlohmann_json
        GITHUB_REPOSITORY nlohmann/json
        VERSION 3.11.3
        OPTIONS "JSON_BuildTests OFF"
    )
endif()

# ============================================================
# Engine (shared library used by the executables and every plugin)
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rtype_engine SHARED
    src/engine/ECS/app/Application.cpp
    src/engine/ECS/app/PluginLoader.cpp
    src/engine/ECS/core/Scene.cpp
    src/engine/ECS/core/TickTimeManager.cpp
    src/engine/ECS/core/EntityManager.cpp
    src/engine/ECS/systems/SystemManager.cpp
    src/engine/ECS/systems/ScriptSystem.cpp
    src/engine/ECS/prefabs/Prefab.cpp
    src/engine/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(rtype_engine PUBLIC
    src/engine/ECS/app
    src/engine/ECS/core
    src/engine/ECS/errors
    src/engine/ECS/prefabs
    src/engine/ECS/systems
    src/engine/ECS/components
    src/engine/ECS/input
    src/engine/ECS/resources
)

target_link_libraries(rtype_engine PRIVATE ${CMAKE_DL_LIBS})

# Asio and PluginLoader pull in windows.h, whose GDI/USER declarations (Rectangle, CloseWindow,
# LoadImage, SendInput...) clash with raylib and our own names. PUBLIC: every target links the engine
target_compile_definitions(rtype_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)

# MSVC exports nothing from a DLL by default: export every engine function for the plugins
set_target_properties(rtype_engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

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
    src/plugins/Renderer/RayLibRenderer.cpp
    src/plugins/Renderer/systems/RenderSystem.cpp
)

target_include_directories(plugin_renderer PUBLIC
    src/plugins/Renderer
    src/plugins/Renderer/components
    src/plugins/Renderer/resources
    src/plugins/Renderer/systems
)

target_include_directories(plugin_renderer SYSTEM PUBLIC
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(plugin_renderer PRIVATE raylib)

add_plugin(plugin_physics Physics
    src/plugins/Physics/Physics.cpp
    src/plugins/Physics/systems/Collider2DSystem.cpp
    src/plugins/Physics/systems/MovementSystem.cpp
)

target_include_directories(plugin_physics PUBLIC
    src/plugins/Physics
    src/plugins/Physics/components
    src/plugins/Physics/systems
)

set(RTYPE_TARGETS
    rtype_engine
    plugin_renderer
    plugin_physics
)

# ============================================================
# R-Type (BUILD_RTYPE): network plugin, game plugins, client and server
# ============================================================

if(BUILD_RTYPE)
    # The R-Type plugins link the Network plugin directly: they call NetTransport and Packet
    add_plugin(plugin_network Network
        src/plugins/Network/NetworkPlugin.cpp
        src/plugins/Network/resources/NetTransport.cpp
        src/plugins/Network/Connection.cpp
        src/plugins/Network/Packet.cpp
        src/plugins/Network/resources/UdpSocket.cpp
        src/plugins/Network/systems/NetReceiveSystem.cpp
        src/plugins/Network/systems/NetSendSystem.cpp
    )

    target_include_directories(plugin_network PUBLIC
        src/plugins/Network
        src/plugins/Network/resources
        src/plugins/Network/systems
    )

    target_link_libraries(plugin_network PUBLIC asio)

    # Exports NetTransport, Packet... for the R-Type plugins (the others only export createPlugin)
    set_target_properties(plugin_network PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

    add_plugin(plugin_rtype_client RTypeClient
        src/games/R-Type/network/client/RTypeClientPlugin.cpp
        src/games/R-Type/network/client/systems/ClientNetSystem.cpp
        src/games/R-Type/network/client/systems/SendInputSystem.cpp
        src/games/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_client PRIVATE
        src/games/R-Type/network/client
        src/games/R-Type/network/client/systems
        src/games/R-Type/network/common
        src/games/R-Type/components
        src/games/R-Type/scripts
    )

    # Only the Renderer and Physics headers (components, resources) are used, not their code
    target_include_directories(plugin_rtype_client PRIVATE
        $<TARGET_PROPERTY:plugin_renderer,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_client PRIVATE plugin_network)

    add_plugin(plugin_rtype_server RTypeServer
        src/games/R-Type/network/server/RTypeServerPlugin.cpp
        src/games/R-Type/network/server/systems/ServerNetSystem.cpp
        src/games/R-Type/network/server/systems/PlayerInputSystem.cpp
        src/games/R-Type/network/server/systems/SnapshotSystem.cpp
        src/games/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_server PRIVATE
        src/games/R-Type/network/server
        src/games/R-Type/network/server/systems
        src/games/R-Type/network/common
        src/games/R-Type/components
        src/games/R-Type/scripts
        $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_server PRIVATE plugin_network)

    # ---- Client ----

    add_executable(${PROJECT_NAME}
        src/games/R-Type/network/client/main.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        src/games/R-Type/network/common
        src/plugins/Network
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE rtype_engine asio)

    # The executables only load the plugins at runtime: build them too, so they are next to the binaries
    add_dependencies(${PROJECT_NAME} plugin_renderer plugin_physics plugin_network plugin_rtype_client)

    # ---- Server ----

    add_executable(r-type_server
        src/games/R-Type/network/server/main.cpp
        src/games/R-Type/network/server/ServerLoop.cpp
    )

    target_include_directories(r-type_server PRIVATE
        src/games/R-Type/network/server
        src/games/R-Type/network/common
        src/plugins/Network
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
endif()

# ============================================================
# Gringo editor (BUILD_GRINGO): ./gringo
# ============================================================

if(BUILD_GRINGO)
    file(GLOB_RECURSE GRINGO_SOURCES CONFIGURE_DEPENDS src/engine/editor/*.cpp)

    add_executable(Gringo ${GRINGO_SOURCES})

    if(DEFINED GRINGO_GAME_TARGET AND NOT "${GRINGO_GAME_TARGET}" STREQUAL "" AND
       DEFINED GRINGO_GAME_SOURCE AND NOT "${GRINGO_GAME_SOURCE}" STREQUAL "")
        add_executable(${GRINGO_GAME_TARGET}
            ${GRINGO_GAME_SOURCE}
            src/engine/runtime/ProjectComponentManager.cpp
        )
        target_include_directories(${GRINGO_GAME_TARGET} PRIVATE
            src/engine/runtime
            src/plugins/Renderer/resources
            src/plugins/Physics/components
            src/plugins/Renderer/components
            ${GRINGO_GAME_SCRIPT_DIR}
        )
        target_link_libraries(${GRINGO_GAME_TARGET} PRIVATE
            rtype_engine nlohmann_json::nlohmann_json
        )
        add_dependencies(${GRINGO_GAME_TARGET} plugin_renderer plugin_physics)
    endif()

    target_include_directories(Gringo PRIVATE
        src/engine/editor
        src/engine/editor/builder
        src/engine/editor/catalog
        src/engine/editor/parser
        src/engine/editor/renderer
        src/engine/editor/menu/AMenu
        src/engine/editor/menu/mainMenu
        src/engine/editor/menu/templateMenu
        src/engine/editor/menu/nameMenu
        src/engine/editor/menu/loadMenu
        src/engine/editor/menu/editorMenu
        src/engine/editor/menu/editorMenu/components
        src/engine/editor/menu/editorMenu/objects
        src/engine/editor/menu/editorMenu/properties
    )

    target_include_directories(Gringo SYSTEM PRIVATE
        "${raylib_SOURCE_DIR}/src"
    )

    # rtype_engine gives the editor Application, PluginLoader and the ECS headers
    # (ComponentDescriptor.hpp, Position.hpp, ColorRGBA.hpp)
    target_link_libraries(Gringo PRIVATE rtype_engine raylib nlohmann_json::nlohmann_json)

    # The editor lists the components of every plugin found in plugins/: build the ones that exist
    add_dependencies(Gringo plugin_renderer plugin_physics)

    if(BUILD_RTYPE)
        add_dependencies(Gringo plugin_network plugin_rtype_client plugin_rtype_server)
    endif()
endif()

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
                "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/Norms/.clang-tidy"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()