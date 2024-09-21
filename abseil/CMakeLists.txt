#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
]]
set(src
    "abseil.cpp"
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


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/abseil)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/abseil)
add_executable(abseil ${ALL_FILES})
set_target_properties(abseil PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(abseil PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(absl CONFIG REQUIRED)

target_link_libraries(abseil PRIVATE 
    absl::log_initialize
    absl::log
    logbench::logbench
)
