#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
cmake_minimum_required(VERSION 3.14)
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
if( CMAKE_CXX_STANDARD VERSION_LESS 17 )
  message( FATAL_ERROR "Minimum C++17 required!" )
else()
	message(STATUS "C++ Standard = ${CMAKE_CXX_STANDARD}")
endif()

#---------------------------
# loglib_benchmarks project
#---------------------------
project(loglib_benchmarks LANGUAGES CXX)

#------------
# Build type
#------------
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
	if (NOT CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES Release;Debug CACHE STRING "Multi-config configurations." FORCE)
	endif()
	set(BUILD_TYPE_MSG "Multi-config (${CMAKE_CONFIGURATION_TYPES})")
	message(STATUS "Genearator: ${CMAKE_GENERATOR} (multi-config).")
else()
	if (NOT CMAKE_BUILD_TYPE)
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type, Release/Debug/..." FORCE)
	endif()
	message(STATUS "Genearator: ${CMAKE_GENERATOR} (single-config).") 
	set(BUILD_TYPE_MSG ${CMAKE_BUILD_TYPE})
endif()

#----------------------
# Print project info
#----------------------
message(STATUS 	"PROJECT ${CMAKE_PROJECT_NAME} ${BUILD_TYPE_MSG}")

#------------
# Output dir 
#------------
set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin$<$<CONFIG:Debug>:_debug>)

#-------------------
# benchmark targets
#-------------------
add_subdirectory(abseil)
add_subdirectory(boost)
add_subdirectory(fstlog)
add_subdirectory(fstlog-mt)
add_subdirectory(fstlog-st)
add_subdirectory(g3log)
add_subdirectory(log4cxx)
add_subdirectory(noop)
add_subdirectory(p7)
add_subdirectory(poco)
add_subdirectory(quill)
add_subdirectory(quill-bounded)
add_subdirectory(spdlog)

#add_subdirectory(fstlog-fast)
#add_subdirectory(fstlog-lowl)
#add_subdirectory(fstlog-stfix)
#add_subdirectory(spdlogv2)
