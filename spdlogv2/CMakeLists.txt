#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
#-------------
# Files
#-------------
set(src "spdlog.cpp")
set(includes "logger.hpp")
set(ALL_FILES ${src} ${includes})

#------------
# IDE Groups
#------------
source_group("Source Files" FILES ${src})
source_group("Header Files" FILES ${includes})

#----------
# Out Dirs
#----------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/spdlogv2)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/spdlogv2)

#--------
# Target
#--------
add_executable(spdlogv2 ${ALL_FILES})
set_target_properties(spdlogv2 PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(spdlogv2 PRIVATE
	"./")
    
##########################
# Fetch spdlogv2
##########################
include(FetchContent)
# With PATCH_COMMAND we patch the spdlog CMake configuration file to prevent polluting our bin directory.
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v2.x
    PATCH_COMMAND  ${CMAKE_COMMAND} -DSOURCE_DIR=<SOURCE_DIR> -P ${CMAKE_CURRENT_SOURCE_DIR}/patch_spdlog.cmake
)

FetchContent_MakeAvailable(spdlog)

################################################################################
# Dependencies
################################################################################
find_package(logbench CONFIG REQUIRED)

target_link_libraries(spdlogv2
    PRIVATE spdlog
    PRIVATE logbench::logbench
)
