cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

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
# Client
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/games/rtype/client/main.cpp
    src/games/rtype/client/RTypeClientPlugin.cpp
    src/games/rtype/client/systems/ClientNetSystem.cpp
    src/games/rtype/client/systems/SendInputSystem.cpp
    src/games/rtype/common/RTypeMessages.cpp
    src/engine/app/Application.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/engine/systems/ThreadPool.cpp
    src/engine/prefabs/Prefab.cpp
    src/engine/prefabs/PrefabsManager.cpp
    src/engine/systems/ScriptSystem.cpp
    src/plugins/Renderer/RayLibRenderer.cpp
    src/plugins/Renderer/systems/RenderSystem.cpp
    src/plugins/Physics/Physics.cpp
    src/plugins/Physics/systems/Collider2DSystem.cpp
    src/plugins/Physics/systems/MovementSystem.cpp
    src/plugins/Network/NetworkPlugin.cpp
    src/plugins/Network/resources/NetTransport.cpp
    src/plugins/Network/Connection.cpp
    src/plugins/Network/Packet.cpp
    src/plugins/Network/resources/UdpSocket.cpp
    src/plugins/Network/systems/NetReceiveSystem.cpp
    src/plugins/Network/systems/NetSendSystem.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/games/rtype/client
    src/games/rtype/client/systems
    src/games/rtype/common
    src/games/rtype/components
    src/games/rtype/scripts
    src/engine/app
    src/engine/core
    src/engine/errors
    src/engine/prefabs
    src/engine/systems
    src/engine/components
    src/engine/input
    src/engine/resources
    src/plugins/Renderer
    src/plugins/Renderer/components
    src/plugins/Renderer/resources
    src/plugins/Renderer/systems
    src/plugins/Physics
    src/plugins/Physics/components
    src/plugins/Physics/systems
    src/plugins/Network
    src/plugins/Network/resources
    src/plugins/Network/systems
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib asio)

# Asio pulls in windows.h, whose GDI/USER declarations (Rectangle, CloseWindow,
# LoadImage, SendInput...) clash with raylib and our own names
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOGDI NOUSER WIN32_LEAN_AND_MEAN)
endif()

# ============================================================
# Compiler warnings
# ============================================================

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# ============================================================
# Server
# ============================================================

add_executable(r-type_server
    src/games/rtype/server/main.cpp
    src/games/rtype/server/ServerLoop.cpp
    src/games/rtype/server/RTypeServerPlugin.cpp
    src/games/rtype/server/systems/ServerNetSystem.cpp
    src/games/rtype/server/systems/PlayerInputSystem.cpp
    src/games/rtype/server/systems/SnapshotSystem.cpp
    src/games/rtype/common/RTypeMessages.cpp
    src/engine/app/Application.cpp
    src/engine/systems/ScriptSystem.cpp
    src/engine/core/Scene.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/engine/systems/ThreadPool.cpp
    src/engine/prefabs/Prefab.cpp
    src/engine/prefabs/PrefabsManager.cpp
    src/plugins/Physics/Physics.cpp
    src/plugins/Physics/systems/Collider2DSystem.cpp
    src/plugins/Physics/systems/MovementSystem.cpp
    src/plugins/Network/NetworkPlugin.cpp
    src/plugins/Network/resources/NetTransport.cpp
    src/plugins/Network/Connection.cpp
    src/plugins/Network/Packet.cpp
    src/plugins/Network/resources/UdpSocket.cpp
    src/plugins/Network/systems/NetReceiveSystem.cpp
    src/plugins/Network/systems/NetSendSystem.cpp
)

target_include_directories(r-type_server PRIVATE
    src/games/rtype/server
    src/games/rtype/server/systems
    src/games/rtype/common
    src/games/rtype/components
    src/games/rtype/scripts
    src/engine/app
    src/engine/core
    src/engine/errors
    src/engine/prefabs
    src/engine/systems
    src/engine/components
    src/engine/input
    src/engine/resources
    src/plugins/Physics
    src/plugins/Physics/components
    src/plugins/Physics/systems
    src/plugins/Network
    src/plugins/Network/resources
    src/plugins/Network/systems
)

target_link_libraries(r-type_server PRIVATE asio)

if(MSVC)
    target_compile_options(r-type_server PRIVATE /W4)
else()
    target_compile_options(r-type_server PRIVATE -Wall -Wextra)
endif()

# ============================================================
# clang-tidy (runs on every compile of the client and server)
# ============================================================

option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" ON)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
        set_target_properties(${PROJECT_NAME} r-type_server PROPERTIES
            CXX_CLANG_TIDY
                "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/Norms/.clang-tidy"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()
