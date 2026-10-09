cmake_minimum_required(VERSION 3.21)
project(rtype CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

# ---------------- Engine library ----------------
add_library(rtype_engine STATIC
    src/engine/core/EntityManager.cpp
    src/engine/core/Scene.cpp
    src/engine/systems/SystemManager.cpp
)

target_include_directories(rtype_engine PUBLIC
    src/engine
    src/engine/core
    src/engine/systems
    src/engine/renderer
    src/engine/errors
)

target_compile_options(rtype_engine PRIVATE -Wall -Wextra)

# ---------------- Game systems library ----------------
add_library(rtype_systems STATIC
    src/systems/MovementSystem.cpp
    src/systems/RenderSystem.cpp
    src/systems/InputSystem.cpp
)

target_include_directories(rtype_systems PUBLIC
    src/components
    src/systems
)

target_link_libraries(rtype_systems PUBLIC rtype_engine)
target_compile_options(rtype_systems PRIVATE -Wall -Wextra)

# ---------------- Executable ----------------
add_executable(rtype_demo
    src/main.cpp
    src/game-rtype/Application.cpp
    src/game-rtype/RaylibRenderer.cpp
)

target_include_directories(rtype_demo PRIVATE
    src/game-rtype
)

target_link_libraries(rtype_demo PRIVATE
    rtype_engine
    rtype_systems
    raylib
)

target_compile_options(rtype_demo PRIVATE -Wall -Wextra)