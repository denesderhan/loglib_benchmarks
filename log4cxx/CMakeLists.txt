#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
]]
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

set(src
    "log4cxx.cpp"
)
source_group("Source Files" FILES ${src})

set(includes
    "logger.hpp"
)

source_group("Header Files" FILES ${includes})

set(ALL_FILES
    ${src}
    ${includes}
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/log4cxx)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/log4cxx)
add_executable(log4cxxb ${ALL_FILES})
set_target_properties(log4cxxb PROPERTIES 
	OUTPUT_NAME "log4cxx"
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(log4cxxb PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(log4cxx REQUIRED)

target_link_libraries(log4cxxb PRIVATE
    log4cxx
    fmt::fmt
    logbench::logbench
)
