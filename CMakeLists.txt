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
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
    OPTIONS "JSON_BuildTests OFF"
)

# ============================================================
# Engine (shared library used by the executables and every plugin)
# ============================================================

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rtype_engine SHARED
    ${GRINGO_ENGINE_DIR}/src/ECS/app/Application.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/app/PluginLoader.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/ComponentManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/Scene.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/TickTimeManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/core/EntityManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/SystemManager.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/systems/ScriptSystem.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/Prefab.cpp
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(rtype_engine PUBLIC
    ${GRINGO_ENGINE_DIR}/src/ECS/app
    ${GRINGO_ENGINE_DIR}/src/ECS/core
    ${GRINGO_ENGINE_DIR}/src/ECS/errors
    ${GRINGO_ENGINE_DIR}/src/ECS/prefabs
    ${GRINGO_ENGINE_DIR}/src/ECS/systems
    ${GRINGO_ENGINE_DIR}/src/ECS/components
    ${GRINGO_ENGINE_DIR}/src/ECS/input
    ${GRINGO_ENGINE_DIR}/src/ECS/resources
)

target_link_libraries(rtype_engine PUBLIC nlohmann_json::nlohmann_json PRIVATE ${CMAKE_DL_LIBS})

# Asio and PluginLoader pull in windows.h, whose GDI/USER declarations (Rectangle, CloseWindow,
# LoadImage, SendInput...) clash with raylib and our own names. PUBLIC: every target links the engine
target_compile_definitions(rtype_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)

# MSVC exports nothing from a DLL by default: export every engine function for the plugins
set_target_properties(rtype_engine PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    LIBRARY_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${GRINGO_ENGINE_DIR}"
)

# ============================================================
# Plugins (one shared library each, loaded at runtime from plugins/)
# ============================================================

# Engine plugins: every folder of Gringo Engine/src/plugins is one plugin: all its .cpp files are compiled into
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

gringo_add_plugins("${GRINGO_ENGINE_DIR}/src/plugins" rtype_engine "${RTYPE_PLUGIN_DIRECTORY}")

set(RTYPE_TARGETS
    rtype_engine
    ${GRINGO_PLUGIN_TARGETS}
)

# ============================================================
# R-Type game plugins, client and server
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

# The R-Type plugins link the Network plugin directly: they call NetTransport and Packet
    add_plugin(plugin_rtype_client RTypeClient
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/RTypeClientPlugin.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems/ClientNetSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems/SendInputSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_client PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/client/systems
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/components
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
    )

    # Only the Renderer and Physics headers (components, resources) are used, not their code
    target_include_directories(plugin_rtype_client PRIVATE
        $<TARGET_PROPERTY:plugin_RayLibRenderer,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:plugin_Physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_client PRIVATE plugin_Network)

    add_plugin(plugin_rtype_server RTypeServer
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/RTypeServerPlugin.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/ServerNetSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/PlayerInputSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems/SnapshotSystem.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common/RTypeMessages.cpp
    )

    target_include_directories(plugin_rtype_server PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/systems
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/components
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
        $<TARGET_PROPERTY:plugin_Physics,INTERFACE_INCLUDE_DIRECTORIES>
    )

    target_link_libraries(plugin_rtype_server PRIVATE plugin_Network)

    # ---- Client ----

    add_executable(${PROJECT_NAME}
        ${GRINGO_REPOSITORY_DIR}/R-Type/main.cpp
        ${GRINGO_ENGINE_DIR}/src/runtime/ProjectComponentManager.cpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GRINGO_ENGINE_DIR}/src/runtime
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
        ${GRINGO_REPOSITORY_DIR}/R-Type/scripts
    )

    # Plugin headers + GRINGO_USE_<NAME> defines, and builds them with the client
    gringo_use_plugins(${PROJECT_NAME} RayLibRenderer Physics Network)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        rtype_engine
        asio
        nlohmann_json::nlohmann_json
    )

    # The executables only load the plugins at runtime: build them too, so they are next to the binaries
    add_dependencies(${PROJECT_NAME} plugin_rtype_client)

    # ---- Server ----

    add_executable(r-type_server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/main.cpp
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server/ServerLoop.cpp
    )

    target_include_directories(r-type_server PRIVATE
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/server
        ${GRINGO_REPOSITORY_DIR}/R-Type/network/common
    )

    target_link_libraries(r-type_server PRIVATE rtype_engine)

    gringo_use_plugins(r-type_server Physics Network)
    add_dependencies(r-type_server plugin_rtype_server)

    list(APPEND RTYPE_TARGETS
        plugin_rtype_client
        plugin_rtype_server
        ${PROJECT_NAME}
        r-type_server
    )
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