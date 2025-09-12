#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
cmake_minimum_required(VERSION 3.21)
message(STATUS "Cmake version = ${CMAKE_VERSION}")

#-------
#OPTIONS
#-------

option(CMAKE_EXPORT_COMPILE_COMMANDS "Export compile commands." ON)
set(CMAKE_DEBUG_POSTFIX d CACHE STRING "Debug filename postfix.")
mark_as_advanced(FORCE CMAKE_DEBUG_POSTFIX CMAKE_EXPORT_COMPILE_COMMANDS)

#----------------------
# C++ standard setting
#----------------------
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard version")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#---------------------------
# loglib_benchmarks project
#---------------------------
project(loglib_benchmarks LANGUAGES CXX)

#------------
# Build type
#------------
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    if(CMAKE_BUILD_TYPE)
        message(FATAL_ERROR "CMAKE_BUILD_TYPE must be empty if using multi-config generator")
    endif()
    if (NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Release;Debug CACHE STRING "Multi-config configurations." FORCE)
    endif()
    set(BUILD_TYPE_MSG "Multi-config (${CMAKE_CONFIGURATION_TYPES})")
    set(GENERATOR_MSG "${CMAKE_GENERATOR} (multi-config)")
else()
    if (NOT CMAKE_BUILD_TYPE)
        if(DEFINED VCPKG_TARGET_TRIPLET)
            message(FATAL_ERROR "vcpkg toolchain detected, you must set -DCMAKE_BUILD_TYPE=Release/Debug... explicitly, to prevent mismatched (Release/Debug) components.") 
        endif()
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type, Release/Debug/..." FORCE)
    endif()
    if(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release|RelWithDebInfo|MinSizeRel)$")
        message(FATAL_ERROR
            "CMAKE_BUILD_TYPE must be one of Debug, Release, RelWithDebInfo, MinSizeRel (got \"${CMAKE_BUILD_TYPE}\").")
    endif()
    set(GENERATOR_MSG "${CMAKE_GENERATOR} (single-config)")
    set(BUILD_TYPE_MSG ${CMAKE_BUILD_TYPE})
endif()

#-------------------
# Benchmark Targets
#-------------------

file(GLOB SUBDIR_CMAKELISTS CONFIGURE_DEPENDS RELATIVE ${CMAKE_SOURCE_DIR}
     "${CMAKE_SOURCE_DIR}/*/CMakeLists.txt")

foreach(path ${SUBDIR_CMAKELISTS})
    get_filename_component(dir ${path} DIRECTORY)
    list(APPEND AVAIL_BENCHMARK_TARGETS "${dir}")
endforeach()

if (NOT DEFINED BENCHMARK_TARGETS)
    set(BENCHMARK_TARGETS "noop;abseil;boost;fstlog-tloc;g3log;log4cxx;p7;poco;quill;spdlog" CACHE STRING "The Benchmarks to build.")
endif()

# Convert to lists for easier comparison
list(REMOVE_DUPLICATES AVAIL_BENCHMARK_TARGETS)
list(REMOVE_DUPLICATES BENCHMARK_TARGETS)

set(SKIPPED_TARGETS)

if("${BENCHMARK_TARGETS}" STREQUAL "all")
    set(BUILT_TARGETS ${AVAIL_BENCHMARK_TARGETS})
else()
    # Find which targets will be built
    set(BUILT_TARGETS)
    foreach(target ${BENCHMARK_TARGETS})
        if(target IN_LIST AVAIL_BENCHMARK_TARGETS)
            list(APPEND BUILT_TARGETS ${target})
        else()
            message(FATAL_ERROR "Unavailable benchmark target: ${target}, must be in ${AVAIL_BENCHMARK_TARGETS}")
        endif()
    endforeach()

    # Find which targets won't be built
    foreach(target ${AVAIL_BENCHMARK_TARGETS})
        if(NOT target IN_LIST BENCHMARK_TARGETS)
            list(APPEND SKIPPED_TARGETS ${target})
        endif()
    endforeach()
endif()

#----------------------
# Print project info
#----------------------
message(STATUS "=================================================================================")
message(STATUS "${PROJECT_NAME} ${BUILD_TYPE_MSG} C++${CMAKE_CXX_STANDARD}")
message(STATUS "---------------------------------------------------------------------------------")
message(STATUS "Generator: ${GENERATOR_MSG}.")
message(STATUS "---------------------------------------------------------------------------------")
message(STATUS "Available targets: ${AVAIL_BENCHMARK_TARGETS}")
message(STATUS "Targets built:     ${BUILT_TARGETS}")
message(STATUS "Targets skipped:   ${SKIPPED_TARGETS}")
message(STATUS "Use \"-DBENCHMARK_TARGETS=noop;boost\" to specify built targets.")
message(STATUS "=================================================================================")

if( CMAKE_CXX_STANDARD VERSION_LESS 17 )
  message( FATAL_ERROR "Minimum C++17 required!" )
endif()

#------------------
# Dependency setup
#------------------
include(FetchContent)
# Try to find the package first
find_package(Threads REQUIRED)
find_package(logbench QUIET)
# If not found, use FetchContent as a fallback
if(NOT logbench_FOUND)
    message(STATUS "find_package() did not find logbench, fetching from github.com")
    FetchContent_Declare(
        logbench
        GIT_REPOSITORY https://github.com/denesderhan/logbench
        GIT_TAG main
        OVERRIDE_FIND_PACKAGE    # override find_package() for subsequent calls
    )
    set(LOGBENCH_TESTS OFF CACHE BOOL "Don't build tests" FORCE)
    FetchContent_MakeAvailable(logbench)
endif()

set(fstlog_needed FALSE)
foreach(item IN LISTS BUILT_TARGETS)
    # Check if item contains "fstlog"
    if(item MATCHES ".*fstlog.*")
        set(fstlog_needed TRUE)
        break()
    endif()
endforeach()

if(fstlog_needed)
    if(NOT DEFINED FETCH_FSTLOG)
        find_package(fstlog QUIET)
        if(NOT fstlog_FOUND)
            set(FETCH_FSTLOG "main")
        else()
            message(STATUS "Found fstlog v${fstlog_VERSION}, using installed package.")
        endif()
    endif()
    if(DEFINED FETCH_FSTLOG)
        if(NOT DEFINED FSTLOG_REPO)
            set(FSTLOG_REPO "https://github.com/denesderhan/fstlog")
        endif()
        message(STATUS "Fetching fstlog from: ${FSTLOG_REPO} with tag: ${FETCH_FSTLOG}")
        FetchContent_Declare(
            fstlog
            GIT_REPOSITORY ${FSTLOG_REPO}
            GIT_TAG ${FETCH_FSTLOG}
            OVERRIDE_FIND_PACKAGE    # override find_package() for subsequent calls
        )
        set(FSTLOG_TESTING OFF CACHE BOOL "Don't build tests" FORCE)
        set(FSTLOG_EXAMPLES OFF CACHE BOOL "Don't build examples" FORCE)
        FetchContent_MakeAvailable(fstlog)
        find_package(fstlog REQUIRED)
        get_target_property(fstlog_VERSION fstlog VERSION)
        message(STATUS "Using fetched fstlog v${fstlog_VERSION}")
    endif()
    # Set the version from the imported fstlog::fstlog target
    set(FSTLOG_VERSION ${fstlog_VERSION})
endif()

#------------
# Output dir 
#------------
set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin$<$<CONFIG:Debug>:_debug>)

#-----------------------------
# configure benchmark targets
#-----------------------------
foreach(target ${BUILT_TARGETS})
    add_subdirectory(${target})
endforeach()
