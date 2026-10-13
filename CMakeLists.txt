cmake_minimum_required(VERSION 3.28)

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
    GIT_TAG 5.5
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_SHARED_LIBS OFF"
)

if(TARGET raylib)
    target_compile_options(raylib PRIVATE -w)
endif()

# ============================================================
# Client
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/game-rtype/Application.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
    src/plugins/Renderer/RayLibRenderer.cpp
    src/plugins/Physics/Physics.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/game-rtype
    src/engine/core
    src/engine/errors
    src/engine/systems
    src/components
    src/common
    src/ressources
    src/plugins/Renderer
    src/plugins/Renderer/components
    src/plugins/Renderer/ressources
    src/plugins/Renderer/systems
    src/plugins/Physics
    src/plugins/Physics/components
    src/plugins/Physics/systems
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${raylib_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

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

find_package(Threads REQUIRED)

add_executable(r-type_server
    src/server/main.cpp
    src/server/server.cpp
    src/server/network/network.cpp
    src/server/game/game_loop.cpp
    src/server/input/input_queue.cpp
    src/engine/core/Scene.cpp
    src/engine/core/TickTimeManager.cpp
    src/engine/core/EntityManager.cpp
    src/engine/systems/SystemManager.cpp
)

target_include_directories(r-type_server PRIVATE
    src
    src/server
    src/engine/core
    src/engine/errors
    src/engine/systems
    src/components
)

target_link_libraries(r-type_server PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(r-type_server PRIVATE /W4)
else()
    target_compile_options(r-type_server PRIVATE -Wall -Wextra)
endif()
