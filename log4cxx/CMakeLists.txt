#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
]]
#-------
# Files
#-------
set(src "log4cxx.cpp")
set(includes "logger.hpp")
set(ALL_FILES ${src} ${includes})

#------------
# IDE Groups
#------------
source_group("Source Files" FILES ${src})
source_group("Header Files" FILES ${includes})

#-------------
# Output Dirs
#-------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/log4cxx)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/log4cxx)

#--------
# Target
#--------
add_executable(log4cxxb ${ALL_FILES})
set_target_properties(log4cxxb PROPERTIES 
	OUTPUT_NAME "log4cxx"
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(log4cxxb
    PRIVATE	"./")

################################################################################
# Dependencies
################################################################################
find_package(fmt CONFIG REQUIRED)
find_package(log4cxx CONFIG REQUIRED)

target_link_libraries(log4cxxb PRIVATE
    fmt::fmt
    logbench::logbench
    log4cxx
)