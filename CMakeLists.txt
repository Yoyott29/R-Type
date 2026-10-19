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
        target_compile_definitions(asio INTERFACE _WIN32_WINNT=0x0A00)
    endif()
endif()

# ============================================================
# Client
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/engine/game-rtype/Application.cpp
    src/scripts/Game.cpp
    src/engine/engine/core/Scene.cpp
    src/engine/engine/core/TickTimeManager.cpp
    src/engine/engine/core/EntityManager.cpp
    src/engine/engine/systems/SystemManager.cpp
    src/engine/engine/prefabs/Prefab.cpp
    src/engine/engine/prefabs/PrefabsManager.cpp
    src/engine/systems/ScriptSystem.cpp
    src/engine/plugins/Renderer/RayLibRenderer.cpp
    src/engine/plugins/Renderer/systems/RenderSystem.cpp
    src/engine/plugins/Physics/Physics.cpp
    src/engine/plugins/Physics/systems/Collider2DSystem.cpp
    src/engine/plugins/Physics/systems/MovementSystem.cpp
    src/engine/plugins/Network/Client/Client.cpp
    src/engine/plugins/Network/Client/systems/ReadSocket.cpp
    src/engine/plugins/Network/Client/systems/SendInput.cpp
    src/engine/plugins/Network/Client/systems/TryConnect.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/engine/game-rtype
    src/scripts
    src/engine/engine/core
    src/engine/engine/errors
    src/engine/engine/prefabs
    src/engine/engine/systems
    src/engine/engine/prefabs
    src/engine/systems
    src/engine/components
    src/engine/common/
    src/engine/ressources
    src/engine/plugins/Renderer
    src/engine/plugins/Renderer/components
    src/engine/plugins/Renderer/ressources
    src/engine/plugins/Renderer/systems
    src/engine/plugins/Physics
    src/engine/plugins/Physics/components
    src/engine/plugins/Physics/systems
    src/engine/plugins/Network/components
    src/engine/plugins/Network/Client
    src/engine/plugins/Network/Client/systems
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
    src/engine/server/main.cpp
    src/engine/server/server.cpp
    src/engine/server/network/network.cpp
    src/engine/server/game/game_loop.cpp
    src/engine/server/input/input_queue.cpp
    src/scripts/Game.cpp
    src/engine/systems/ScriptSystem.cpp
    src/engine/plugins/Physics/systems/Collider2DSystem.cpp
    src/engine/plugins/Physics/systems/MovementSystem.cpp
    src/engine/engine/core/Scene.cpp
    src/engine/engine/core/TickTimeManager.cpp
    src/engine/engine/core/EntityManager.cpp
    src/engine/engine/systems/SystemManager.cpp
    src/engine/engine/prefabs/Prefab.cpp
    src/engine/engine/prefabs/PrefabsManager.cpp
)

target_include_directories(r-type_server PRIVATE
    src/engine/
    src/engine/game-rtype
    src/scripts
    src/engine/common
    src/engine/server
    src/engine/engine/core
    src/engine/engine/errors
    src/engine/engine/prefabs
    src/engine/engine/systems
    src/engine/systems
    src/engine/components
    src/engine/plugins/Physics/components
    src/engine/plugins/Physics/systems
    src/engine/plugins/Renderer/components
    src/engine/plugins/Renderer/systems
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
