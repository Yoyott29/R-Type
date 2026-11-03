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

# Every folder of src/plugins is one plugin: all its .cpp files are compiled into
# plugins/<name>.so (.dll on Windows), every folder holding a .hpp is an include directory and
# <name> is the folder name. A new plugin needs nothing else. A plugin needing more sets it here,
# keyed by its folder name:
#   <Folder>_PLUGIN_NAME        library name, the name used in .gringo files (default: <Folder>)
#   <Folder>_PUBLIC_LIBRARIES   libraries its headers need (also given to the games using it)
#   <Folder>_PRIVATE_LIBRARIES  libraries only its code needs
#   <Folder>_SYSTEM_INCLUDES    extra include directories, warnings disabled
set(Renderer_PLUGIN_NAME RayLibRenderer)
set(Renderer_PRIVATE_LIBRARIES raylib)
set(Renderer_SYSTEM_INCLUDES "${raylib_SOURCE_DIR}/src")
set(Network_PUBLIC_LIBRARIES asio)
# UI draws through the renderer (IRenderer.hpp)
set(UI_PRIVATE_LIBRARIES plugin_RayLibRenderer)

# gringo_add_plugins(<plugins source dir> <engine target> <output dir>) creates plugin_<name> for
# each plugin and sets GRINGO_PLUGIN_TARGETS in the caller scope
function(gringo_add_plugins plugins_directory engine output_directory)
    file(GLOB plugin_directories LIST_DIRECTORIES true "${plugins_directory}/*")
    list(SORT plugin_directories)
    set(targets "")

    foreach(directory IN LISTS plugin_directories)
        if(NOT IS_DIRECTORY "${directory}")
            continue()
        endif()
        get_filename_component(folder "${directory}" NAME)
        set(name "${folder}")
        if(DEFINED ${folder}_PLUGIN_NAME)
            set(name "${${folder}_PLUGIN_NAME}")
        endif()

        file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${directory}/*.cpp")
        if(NOT sources)
            message(WARNING "Gringo plugin ${folder} has no .cpp file: skipped")
            continue()
        endif()
        file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${directory}/*.hpp")
        set(include_directories "${directory}")
        foreach(header IN LISTS headers)
            get_filename_component(header_directory "${header}" DIRECTORY)
            list(APPEND include_directories "${header_directory}")
        endforeach()
        list(REMOVE_DUPLICATES include_directories)

        set(target "plugin_${name}")
        add_library(${target} SHARED ${sources})
        set_target_properties(${target} PROPERTIES
            OUTPUT_NAME "${name}"
            PREFIX ""
            LIBRARY_OUTPUT_DIRECTORY "${output_directory}"
            RUNTIME_OUTPUT_DIRECTORY "${output_directory}"
            INSTALL_RPATH "\$ORIGIN;\$ORIGIN/.."
            # Lets another plugin link this one and call its functions (e.g. Network)
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
        set_property(TARGET ${target} PROPERTY GRINGO_PLUGIN_FOLDER "${folder}")
        set_property(TARGET ${target} PROPERTY GRINGO_PUBLIC_LIBRARIES ${${folder}_PUBLIC_LIBRARIES})
        target_include_directories(${target} PUBLIC ${include_directories})
        if(${folder}_SYSTEM_INCLUDES)
            target_include_directories(${target} SYSTEM PUBLIC ${${folder}_SYSTEM_INCLUDES})
        endif()
        target_link_libraries(${target}
            PUBLIC ${engine} ${${folder}_PUBLIC_LIBRARIES}
            PRIVATE ${${folder}_PRIVATE_LIBRARIES}
        )
        list(APPEND targets "${target}")
    endforeach()

    set(GRINGO_PLUGIN_TARGETS ${targets} PARENT_SCOPE)
endfunction()

# gringo_use_plugins(<target> <plugin names...>) gives an executable the headers of the plugins it
# uses (it still loads them at runtime), builds them with it and defines GRINGO_USE_<NAME> and
# GRINGO_USE_<FOLDER>. Names that are not engine plugins (game plugins) are skipped
function(gringo_use_plugins target)
    foreach(name IN LISTS ARGN)
        if(NOT TARGET plugin_${name})
            message(STATUS "${target}: ${name} is not an engine plugin, loaded at runtime only")
            continue()
        endif()
        target_include_directories(${target} PRIVATE
            $<TARGET_PROPERTY:plugin_${name},INTERFACE_INCLUDE_DIRECTORIES>
        )
        get_property(libraries TARGET plugin_${name} PROPERTY GRINGO_PUBLIC_LIBRARIES)
        if(libraries)
            target_link_libraries(${target} PRIVATE ${libraries})
        endif()
        get_property(folder TARGET plugin_${name} PROPERTY GRINGO_PLUGIN_FOLDER)
        string(MAKE_C_IDENTIFIER "${name}" name_define)
        string(MAKE_C_IDENTIFIER "${folder}" folder_define)
        string(TOUPPER "${name_define}" name_define)
        string(TOUPPER "${folder_define}" folder_define)
        target_compile_definitions(${target} PRIVATE
            GRINGO_USE_${name_define} GRINGO_USE_${folder_define}
        )
        add_dependencies(${target} plugin_${name})
    endforeach()
endfunction()

gringo_add_plugins("${CMAKE_CURRENT_SOURCE_DIR}/src/plugins" gringo_engine
                   "${GRINGO_PLUGIN_DIRECTORY}")

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
add_dependencies(Gringo ${GRINGO_PLUGIN_TARGETS})

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

    # GRINGO_GAME_PLUGINS: the plugins active in the project (headers + GRINGO_USE_<NAME>)
    gringo_use_plugins(${GRINGO_GAME_TARGET} ${GRINGO_GAME_PLUGINS})
endif()

set(GRINGO_TARGETS gringo_engine ${GRINGO_PLUGIN_TARGETS} Gringo)
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
