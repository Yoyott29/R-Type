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
set(RTYPE_RENDERER_DIRECTORY "${GRINGO_ENGINE_DIR}/renderers$<0:>")

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
    NAME miniaudio
    GITHUB_REPOSITORY mackron/miniaudio
    GIT_TAG 0.11.22
    DOWNLOAD_ONLY YES
)

if(miniaudio_ADDED)
    add_library(miniaudio INTERFACE)
    target_include_directories(miniaudio SYSTEM INTERFACE "${miniaudio_SOURCE_DIR}")
    target_link_libraries(miniaudio INTERFACE Threads::Threads ${CMAKE_DL_LIBS})
    if(UNIX)
        target_link_libraries(miniaudio INTERFACE m)
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

add_library(gringo_engine SHARED
    ${GRINGO_ENGINE_DIR}/src/ECS/app/Application.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/app/PluginLoader.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/ComponentManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/Scene.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/TickTimeManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/EntityManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/ThreadPool.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/ParallelFor.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/ScheduleGraph.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/ScheduleRunner.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/SystemManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/Prefab.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(gringo_engine PUBLIC
    ${GRINGO_ENGINE_DIR}/src/ECS/app
    ${GRINGO_ENGINE_DIR}/src/ECS/core
    ${GRINGO_ENGINE_DIR}/src/ECS/errors
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs
    ${GRINGO_ENGINE_DIR}/src/ECS/systems
    ${GRINGO_ENGINE_DIR}/src/ECS/components
    ${GRINGO_ENGINE_DIR}/src/ECS/input
    ${GRINGO_ENGINE_DIR}/src/ECS/resources
    ${GRINGO_ENGINE_DIR}/src/renderers
)

target_link_libraries(gringo_engine PUBLIC nlohmann_json::nlohmann_json PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)

# Asio and PluginLoader pull in windows.h, whose GDI/USER declarations (Rectangle, CloseWindow,
# LoadImage, SendInput...) clash with raylib and our own names. PUBLIC: every target links the engine
target_compile_definitions(gringo_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)

# MSVC exports nothing from a DLL by default: export every engine function for the plugins
set_target_properties(gringo_engine PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
)

# ============================================================
# Plugins (one shared library each, loaded at runtime from plugins/)
# ============================================================

# Every folder of src/plugins is one plugin, and every folder of src/renderers one renderer:
# all its .cpp files are compiled into plugins/<Folder>.so (renderers/<Folder>.so), every folder
# holding a .hpp is an include directory. A new plugin or renderer needs nothing in this file:
# every renderer gets the graphics libraries (raylib), every plugin the networking one (asio).
# A static library a target does not use adds nothing to it

# gringo_add_plugins(<source dir> <engine target> <output dir> <prefix>
#                    [PUBLIC_LIBRARIES ...] [PRIVATE_LIBRARIES ...] [SYSTEM_INCLUDES ...])
# creates <prefix>_<Folder> for each folder and appends the targets to GRINGO_PLUGIN_TARGETS
function(gringo_add_plugins plugins_directory engine output_directory prefix)
    cmake_parse_arguments(ARG "" "" "PUBLIC_LIBRARIES;PRIVATE_LIBRARIES;SYSTEM_INCLUDES" ${ARGN})
    file(GLOB plugin_directories LIST_DIRECTORIES true "${plugins_directory}/*")
    list(SORT plugin_directories)
    set(targets "")

    foreach(directory IN LISTS plugin_directories)
        if(NOT IS_DIRECTORY "${directory}")
            continue()
        endif()
        get_filename_component(name "${directory}" NAME)

        file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${directory}/*.cpp")
        if(NOT sources)
            message(WARNING "Gringo ${prefix} ${name} has no .cpp file: skipped")
            continue()
        endif()
        file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${directory}/*.hpp")
        set(include_directories "${directory}")
        foreach(header IN LISTS headers)
            get_filename_component(header_directory "${header}" DIRECTORY)
            list(APPEND include_directories "${header_directory}")
        endforeach()
        list(REMOVE_DUPLICATES include_directories)

        set(target "${prefix}_${name}")
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
        set_property(TARGET ${target} PROPERTY GRINGO_PUBLIC_LIBRARIES ${ARG_PUBLIC_LIBRARIES})
        target_include_directories(${target} PUBLIC ${include_directories})
        if(ARG_SYSTEM_INCLUDES)
            target_include_directories(${target} SYSTEM PRIVATE ${ARG_SYSTEM_INCLUDES})
        endif()
        target_link_libraries(${target}
            PUBLIC ${engine} ${ARG_PUBLIC_LIBRARIES}
            PRIVATE ${ARG_PRIVATE_LIBRARIES}
        )
        list(APPEND targets "${target}")
    endforeach()

    set(GRINGO_PLUGIN_TARGETS ${GRINGO_PLUGIN_TARGETS} ${targets} PARENT_SCOPE)
endfunction()

# gringo_use_plugins(<target> <plugin names...>) gives an executable the headers of the plugins it
# uses (it still loads them at runtime), builds them with it and defines GRINGO_USE_<NAME>.
# Names that are not engine plugins (game plugins) are skipped
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
        string(MAKE_C_IDENTIFIER "${name}" define)
        string(TOUPPER "${define}" define)
        target_compile_definitions(${target} PRIVATE GRINGO_USE_${define})
        add_dependencies(${target} plugin_${name})
    endforeach()
endfunction()

gringo_add_plugins("${GRINGO_ENGINE_DIR}/src/plugins" gringo_engine "${RTYPE_PLUGIN_DIRECTORY}" plugin
                   PUBLIC_LIBRARIES asio miniaudio)
gringo_add_plugins("${GRINGO_ENGINE_DIR}/src/renderers" gringo_engine "${RTYPE_RENDERER_DIRECTORY}" renderer
                   PRIVATE_LIBRARIES raylib SYSTEM_INCLUDES "${raylib_SOURCE_DIR}/src")


set(RTYPE_TARGETS
    gringo_engine
    ${GRINGO_PLUGIN_TARGETS}
)

# ============================================================
# R-Type client and server
# ============================================================

# R-Type/client_main.cpp and R-Type/server_main.cpp are generated by the editor from
# R-Type/R-Type.gringo (one per build variant of the Network plugin). After changing the scene,
# its plugins or its scripts, regenerate them with: "Gringo Engine/Gringo" --build R-Type/R-Type.gringo

# rtype_add_game(<target> <source> <renderer or ""> <plugins...>) builds one R-Type executable at
# the repository root, with the headers of the plugins it uses (still loaded at runtime)
function(rtype_add_game target source renderer)
    add_executable(${target}
        ${source}
        ${GRINGO_ENGINE_DIR}/src/runtime/ProjectComponentManager.cpp
    )
    target_include_directories(${target} PRIVATE
        ${GRINGO_ENGINE_DIR}/src/runtime
        ${GRINGO_REPOSITORY_DIR}/R-Type/components
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
    )
    target_link_libraries(${target} PRIVATE gringo_engine nlohmann_json::nlohmann_json)
    # Plugin headers + GRINGO_USE_<NAME> defines, and builds them with the executable
    gringo_use_plugins(${target} ${ARGN})
    # The renderer is only loaded at runtime: build it too, so it is next to the binaries
    if(renderer)
        add_dependencies(${target} renderer_${renderer})
    endif()
endfunction()

foreach(variant client server)
    if(NOT EXISTS ${GRINGO_REPOSITORY_DIR}/R-Type/${variant}_main.cpp)
        message(FATAL_ERROR "R-Type/${variant}_main.cpp is missing: generate it with "
                            "\"Gringo Engine/Gringo\" --build R-Type/R-Type.gringo")
    endif()
endforeach()

# Same plugins as R-Type.gringo; the headless server skips the ones needing a renderer
rtype_add_game(r-type_client ${GRINGO_REPOSITORY_DIR}/R-Type/client_main.cpp RaylibRenderer
               Physics2D 2D Scripts InputHandler Network Audio UI)
rtype_add_game(r-type_server ${GRINGO_REPOSITORY_DIR}/R-Type/server_main.cpp ""
               Physics2D Scripts Network UI)

list(APPEND RTYPE_TARGETS r-type_client r-type_server)
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