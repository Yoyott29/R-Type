cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(gringo_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH "\$ORIGIN")

set(GRINGO_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(GRINGO_PLUGIN_DIRECTORY "${GRINGO_OUTPUT_DIRECTORY}/plugins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GRINGO_OUTPUT_DIRECTORY}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${GRINGO_OUTPUT_DIRECTORY}")

set(CPM_SOURCE_CACHE
    "${CMAKE_CURRENT_SOURCE_DIR}/.cpm-cache"
    CACHE PATH "CPM download cache"
)
include("${CMAKE_CURRENT_SOURCE_DIR}/CPM.cmake")

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

CPMAddPackage(
    NAME asio
    VERSION 1.30.2
    GITHUB_REPOSITORY chriskohlhoff/asio
    GIT_TAG asio-1-30-2
    DOWNLOAD_ONLY YES
)

find_package(Threads REQUIRED)

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

add_library(gringo_engine SHARED
    src/ECS/app/Application.cpp
    src/ECS/app/PluginLoader.cpp
    src/ECS/core/ComponentManager.cpp
    src/ECS/core/Scene.cpp
    src/ECS/core/TickTimeManager.cpp
    src/ECS/core/EntityManager.cpp
    src/ECS/systems/SystemManager.cpp
    src/ECS/systems/ScriptSystem.cpp
    src/ECS/prefabs/Prefab.cpp
    src/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(gringo_engine PUBLIC
    src/ECS/app src/ECS/core src/ECS/errors src/ECS/prefabs src/ECS/systems
    src/ECS/components src/ECS/input src/ECS/resources
)
target_link_libraries(gringo_engine PUBLIC nlohmann_json::nlohmann_json PRIVATE ${CMAKE_DL_LIBS})
target_compile_definitions(gringo_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)
set_target_properties(gringo_engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

function(add_gringo_plugin target name)
    add_library(${target} SHARED ${ARGN})
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${name}"
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${GRINGO_PLUGIN_DIRECTORY}"
        RUNTIME_OUTPUT_DIRECTORY "${GRINGO_PLUGIN_DIRECTORY}"
        INSTALL_RPATH "\$ORIGIN;\$ORIGIN/.."
    )
    target_link_libraries(${target} PUBLIC gringo_engine)
endfunction()

add_gringo_plugin(plugin_renderer RayLibRenderer
    src/plugins/Renderer/RayLibRenderer.cpp
    src/plugins/Renderer/systems/RenderSystem.cpp
)
target_include_directories(plugin_renderer PUBLIC
    src/plugins/Renderer src/plugins/Renderer/components
    src/plugins/Renderer/resources src/plugins/Renderer/systems
)
target_include_directories(plugin_renderer SYSTEM PUBLIC "${raylib_SOURCE_DIR}/src")
target_link_libraries(plugin_renderer PRIVATE raylib)

add_gringo_plugin(plugin_physics Physics
    src/plugins/Physics/Physics.cpp
    src/plugins/Physics/systems/Collider2DSystem.cpp
    src/plugins/Physics/systems/MovementSystem.cpp
)
target_include_directories(plugin_physics PUBLIC
    src/plugins/Physics src/plugins/Physics/components src/plugins/Physics/systems
)

add_gringo_plugin(plugin_network Network
    src/plugins/Network/NetworkPlugin.cpp
    src/plugins/Network/resources/NetTransport.cpp
    src/plugins/Network/Connection.cpp
    src/plugins/Network/Packet.cpp
    src/plugins/Network/resources/UdpSocket.cpp
    src/plugins/Network/systems/NetReceiveSystem.cpp
    src/plugins/Network/systems/NetSendSystem.cpp
)
target_include_directories(plugin_network PUBLIC
    src/plugins/Network src/plugins/Network/resources src/plugins/Network/systems
)
target_link_libraries(plugin_network PUBLIC asio)
set_target_properties(plugin_network PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE GRINGO_EDITOR_SOURCES CONFIGURE_DEPENDS src/editor/*.cpp)
add_executable(Gringo ${GRINGO_EDITOR_SOURCES})
target_include_directories(Gringo PRIVATE
    src/editor src/editor/builder src/editor/catalog src/editor/parser src/editor/renderer
    src/editor/menu/AMenu src/editor/menu/mainMenu src/editor/menu/templateMenu
    src/editor/menu/nameMenu src/editor/menu/loadMenu src/editor/menu/editorMenu
    src/editor/menu/editorMenu/components src/editor/menu/editorMenu/objects
    src/editor/menu/editorMenu/properties
)
target_include_directories(Gringo SYSTEM PRIVATE "${raylib_SOURCE_DIR}/src")
target_link_libraries(Gringo PRIVATE gringo_engine raylib nlohmann_json::nlohmann_json)
add_dependencies(Gringo plugin_renderer plugin_physics plugin_network)

if(DEFINED GRINGO_GAME_TARGET AND NOT "${GRINGO_GAME_TARGET}" STREQUAL "" AND
   DEFINED GRINGO_GAME_SOURCE AND NOT "${GRINGO_GAME_SOURCE}" STREQUAL "")
    add_executable(${GRINGO_GAME_TARGET}
        "${GRINGO_GAME_SOURCE}"
        src/runtime/ProjectComponentManager.cpp
    )
    target_include_directories(${GRINGO_GAME_TARGET} PRIVATE
        src/runtime "${GRINGO_GAME_SCRIPT_DIR}"
    )
    if(DEFINED GRINGO_GAME_OUTPUT_DIR AND NOT "${GRINGO_GAME_OUTPUT_DIR}" STREQUAL "")
        set_target_properties(${GRINGO_GAME_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${GRINGO_GAME_OUTPUT_DIR}"
            INSTALL_RPATH "\$ORIGIN/..;\$ORIGIN/../Gringo Engine"
        )
    endif()
    target_link_libraries(${GRINGO_GAME_TARGET} PRIVATE
        gringo_engine nlohmann_json::nlohmann_json
    )

    if(GRINGO_GAME_USE_RENDERER)
        target_include_directories(${GRINGO_GAME_TARGET} PRIVATE
            src/plugins/Renderer/resources src/plugins/Renderer/components
        )
        target_compile_definitions(${GRINGO_GAME_TARGET} PRIVATE GRINGO_USE_RENDERER)
        add_dependencies(${GRINGO_GAME_TARGET} plugin_renderer)
    endif()
    if(GRINGO_GAME_USE_PHYSICS)
        target_include_directories(${GRINGO_GAME_TARGET} PRIVATE src/plugins/Physics/components)
        target_compile_definitions(${GRINGO_GAME_TARGET} PRIVATE GRINGO_USE_PHYSICS)
        add_dependencies(${GRINGO_GAME_TARGET} plugin_physics)
    endif()
    if(GRINGO_GAME_USE_NETWORK)
        target_include_directories(${GRINGO_GAME_TARGET} PRIVATE src/plugins/Network)
        target_link_libraries(${GRINGO_GAME_TARGET} PRIVATE asio)
        add_dependencies(${GRINGO_GAME_TARGET} plugin_network)
    endif()
endif()

set(GRINGO_TARGETS
    gringo_engine plugin_renderer plugin_physics plugin_network Gringo
)
if(DEFINED GRINGO_GAME_TARGET AND NOT "${GRINGO_GAME_TARGET}" STREQUAL "" AND
   TARGET ${GRINGO_GAME_TARGET})
    list(APPEND GRINGO_TARGETS ${GRINGO_GAME_TARGET})
endif()

if(MSVC)
    set(GRINGO_WARNING_FLAGS /W4)
else()
    set(GRINGO_WARNING_FLAGS -Wall -Wextra)
endif()
foreach(target IN LISTS GRINGO_TARGETS)
    if(TARGET ${target})
        target_compile_options(${target} PRIVATE ${GRINGO_WARNING_FLAGS})
    endif()
endforeach()

option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" ON)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set_target_properties(${GRINGO_TARGETS} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE}"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()
