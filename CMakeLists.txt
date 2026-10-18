cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
    endif()
endif()

# ============================================================
# Client
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/game-rtype/Application.cpp
    src/game-rtype/Game.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/engine/prefabs/Prefab.cpp
    src/engine/prefabs/PrefabsManager.cpp
    src/systems/ScriptSystem.cpp
    src/plugins/renderer/RayLibRenderer.cpp
    src/plugins/renderer/systems/RenderSystem.cpp
    src/plugins/physics/Physics.cpp
    src/plugins/physics/systems/Collider2DSystem.cpp
    src/plugins/physics/systems/MovementSystem.cpp
    src/plugins/network/client/Client.cpp
    src/plugins/network/client/systems/ReadSocket.cpp
    src/plugins/network/client/systems/SendInput.cpp
    src/plugins/network/client/systems/TryConnect.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/game-rtype
    src/engine/core
    src/engine/errors
    src/engine/prefabs
    src/engine/systems
    src/engine/prefabs
    src/systems
    src/components
    src/common/
    src/ressources
    src/plugins/renderer
    src/plugins/renderer/components
    src/plugins/renderer/ressources
    src/plugins/renderer/systems
    src/plugins/physics
    src/plugins/physics/components
    src/plugins/physics/systems
    src/plugins/network/components
    src/plugins/network/client
    src/plugins/network/client/systems
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib asio)

# ============================================================
# Compiler warnings
# ============================================================

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# ============================================================
# clang-tidy (runs on every compile of the client)
# ============================================================

option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" ON)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_CLANG_TIDY
                "${CLANG_TIDY_EXE};--config-file=${CMAKE_SOURCE_DIR}/Norms/.clang-tidy"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()

# ============================================================
# Server
# ============================================================

add_executable(r-type_server
    src/server/main.cpp
    src/server/Server.cpp
    src/server/network/Network.cpp
    src/server/game/GameLoop.cpp
    src/server/input/InputQueue.cpp
    src/game-rtype/Game.cpp
    src/systems/ScriptSystem.cpp
    src/plugins/physics/systems/Collider2DSystem.cpp
    src/plugins/physics/systems/MovementSystem.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/engine/prefabs/Prefab.cpp
    src/engine/prefabs/PrefabsManager.cpp
)

target_include_directories(r-type_server PRIVATE
    src
    src/game-rtype
    src/common
    src/server
    src/server/game
    src/server/input
    src/server/network
    src/engine/core
    src/engine/errors
    src/engine/prefabs
    src/engine/systems
    src/systems
    src/components
    src/plugins/physics/components
    src/plugins/physics/systems
    src/plugins/renderer/components
    src/plugins/renderer/systems
)

target_include_directories(r-type_server SYSTEM PRIVATE
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(r-type_server PRIVATE asio)

if(MSVC)
    target_compile_options(r-type_server PRIVATE /W4)
else()
    target_compile_options(r-type_server PRIVATE -Wall -Wextra)
endif()
