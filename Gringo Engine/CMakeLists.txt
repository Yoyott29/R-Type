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
set(GRINGO_RENDERER_DIRECTORY "${GRINGO_OUTPUT_DIRECTORY}/renderers")
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
    target_compile_definitions(raylib PRIVATE SUPPORT_FILEFORMAT_JPG=1)
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
    NAME miniaudio
    GITHUB_REPOSITORY mackron/miniaudio
    GIT_TAG 0.11.22
    DOWNLOAD_ONLY YES
)

# miniaudio.c is the library's own file that compiles its code (miniaudio.h with
# MINIAUDIO_IMPLEMENTATION), built as C++ like the rest of the project. Position independent: it
# is linked into the Audio plugin's .so
if(miniaudio_ADDED)
    add_library(miniaudio STATIC "${miniaudio_SOURCE_DIR}/miniaudio.c")
    set_source_files_properties("${miniaudio_SOURCE_DIR}/miniaudio.c" PROPERTIES LANGUAGE CXX)
    set_target_properties(miniaudio PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_include_directories(miniaudio SYSTEM PUBLIC "${miniaudio_SOURCE_DIR}")
    target_link_libraries(miniaudio PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
    if(UNIX)
        target_link_libraries(miniaudio PUBLIC m)
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
    src/ECS/core/ThreadPool.cpp
    src/ECS/core/ParallelFor.cpp
    src/ECS/systems/ScheduleGraph.cpp
    src/ECS/systems/ScheduleRunner.cpp
    src/ECS/systems/SystemManager.cpp
    src/ECS/prefabs/Prefab.cpp
    src/ECS/prefabs/PrefabsManager.cpp
)

target_include_directories(gringo_engine PUBLIC
    src/ECS/app src/ECS/core src/ECS/errors src/ECS/prefabs src/ECS/systems
    src/ECS/components src/ECS/input src/ECS/resources src/renderers
)
target_link_libraries(gringo_engine PUBLIC nlohmann_json::nlohmann_json PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)
target_compile_definitions(gringo_engine PUBLIC
    "$<$<PLATFORM_ID:Windows>:NOGDI;NOUSER;WIN32_LEAN_AND_MEAN>"
)
set_target_properties(gringo_engine PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

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

gringo_add_plugins("${CMAKE_CURRENT_SOURCE_DIR}/src/plugins" gringo_engine "${GRINGO_PLUGIN_DIRECTORY}" plugin
                   PUBLIC_LIBRARIES asio miniaudio)
gringo_add_plugins("${CMAKE_CURRENT_SOURCE_DIR}/src/renderers" gringo_engine "${GRINGO_RENDERER_DIRECTORY}" renderer
                   PRIVATE_LIBRARIES raylib SYSTEM_INCLUDES "${raylib_SOURCE_DIR}/src")


file(GLOB_RECURSE GRINGO_EDITOR_SOURCES CONFIGURE_DEPENDS src/editor/*.cpp)
add_executable(Gringo ${GRINGO_EDITOR_SOURCES})
target_include_directories(Gringo PRIVATE
    src/editor src/editor/builder src/editor/catalog src/editor/parser src/editor/renderer
    src/editor/menu/AMenu src/editor/menu/mainMenu src/editor/menu/templateMenu
    src/editor/menu/nameMenu src/editor/menu/loadMenu src/editor/menu/editorMenu
    src/editor/menu/editorMenu/components src/editor/menu/editorMenu/objects
    src/editor/menu/editorMenu/properties src/editor/menu/editorMenu/animations
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
    # GRINGO_GAME_RENDERER: the project's renderer, loaded at runtime, built with the game
    if(TARGET renderer_${GRINGO_GAME_RENDERER})
        add_dependencies(${GRINGO_GAME_TARGET} renderer_${GRINGO_GAME_RENDERER})
    endif()
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

option(GRINGO_BUILD_TESTS "Build the engine tests (CTest)" OFF)
if(GRINGO_BUILD_TESTS)
    enable_testing()
    file(GLOB GRINGO_NETWORK_TEST_SOURCES CONFIGURE_DEPENDS tests/network/*.cpp)
    add_executable(network_tests ${GRINGO_NETWORK_TEST_SOURCES} src/runtime/ProjectComponentManager.cpp)
    target_include_directories(network_tests PRIVATE tests/network src/runtime)
    target_link_libraries(network_tests PRIVATE plugin_Network plugin_InputHandler)
    set_target_properties(network_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        INSTALL_RPATH "${GRINGO_OUTPUT_DIRECTORY};${GRINGO_PLUGIN_DIRECTORY}")
    add_test(NAME network_tests COMMAND network_tests)

    file(GLOB GRINGO_ENGINE_TEST_SOURCES CONFIGURE_DEPENDS tests/engine/*.cpp)
    add_executable(engine_tests ${GRINGO_ENGINE_TEST_SOURCES})
    target_include_directories(engine_tests PRIVATE tests/network)
    target_link_libraries(engine_tests PRIVATE gringo_engine plugin_Physics2D)
    set_target_properties(engine_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        INSTALL_RPATH "${GRINGO_OUTPUT_DIRECTORY};${GRINGO_PLUGIN_DIRECTORY}")
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
