cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The $<0:> generator expression stops multi-config generators (Visual Studio) from adding a
# Release/ or Debug/ subfolder, so the executables always sit next to the plugins/ folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}$<0:>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}$<0:>)
set(RTYPE_PLUGIN_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins$<0:>)

# Static dependencies (raylib) are linked into the plugin libraries, so everything is built as PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# On Linux, the executables find the engine next to them ($ORIGIN) and the plugins find the other
# plugins and the engine one folder up, so the binaries still run once moved
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

# ============================================================
# Engine (shared library used by the executables and every plugin)
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rtype_engine SHARED
    src/engine/app/Application.cpp
    src/engine/app/PluginLoader.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/engine/systems/ScriptSystem.cpp
    src/engine/prefabs/Prefab.cpp
    src/engine/prefabs/PrefabsManager.cpp
)

target_include_directories(rtype_engine PUBLIC
    src/engine/app
    src/engine/core
    src/engine/errors
    src/engine/prefabs
    src/engine/systems
    src/engine/components
    src/engine/input
    src/engine/resources
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
    src/games/rtype/client/RTypeClientPlugin.cpp
    src/games/rtype/client/systems/ClientNetSystem.cpp
    src/games/rtype/client/systems/SendInputSystem.cpp
    src/games/rtype/common/RTypeMessages.cpp
)

target_include_directories(plugin_rtype_client PRIVATE
    src/games/rtype/client
    src/games/rtype/client/systems
    src/games/rtype/common
    src/games/rtype/components
    src/games/rtype/scripts
)

# Only the Renderer and Physics headers (components, resources) are used, not their code
target_include_directories(plugin_rtype_client PRIVATE
    $<TARGET_PROPERTY:plugin_renderer,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(plugin_rtype_client PRIVATE plugin_network)

add_plugin(plugin_rtype_server RTypeServer
    src/games/rtype/server/RTypeServerPlugin.cpp
    src/games/rtype/server/systems/ServerNetSystem.cpp
    src/games/rtype/server/systems/PlayerInputSystem.cpp
    src/games/rtype/server/systems/SnapshotSystem.cpp
    src/games/rtype/common/RTypeMessages.cpp
)

target_include_directories(plugin_rtype_server PRIVATE
    src/games/rtype/server
    src/games/rtype/server/systems
    src/games/rtype/common
    src/games/rtype/components
    src/games/rtype/scripts
    $<TARGET_PROPERTY:plugin_physics,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(plugin_rtype_server PRIVATE plugin_network)

# ============================================================
# Client
# ============================================================

add_executable(${PROJECT_NAME}
    src/games/rtype/client/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/games/rtype/common
    src/plugins/Network
)

target_link_libraries(${PROJECT_NAME} PRIVATE rtype_engine asio)

# The executables only load the plugins at runtime: build them too, so they are next to the binaries
add_dependencies(${PROJECT_NAME} plugin_renderer plugin_physics plugin_network plugin_rtype_client)

# ============================================================
# Server
# ============================================================

add_executable(r-type_server
    src/games/rtype/server/main.cpp
    src/games/rtype/server/ServerLoop.cpp
)

target_include_directories(r-type_server PRIVATE
    src/games/rtype/server
    src/games/rtype/common
    src/plugins/Network
)

target_link_libraries(r-type_server PRIVATE rtype_engine asio)

add_dependencies(r-type_server plugin_physics plugin_network plugin_rtype_server)

# ============================================================
# Compiler warnings
# ============================================================

set(RTYPE_TARGETS
    rtype_engine
    plugin_renderer
    plugin_physics
    plugin_network
    plugin_rtype_client
    plugin_rtype_server
    ${PROJECT_NAME}
    r-type_server
)

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
