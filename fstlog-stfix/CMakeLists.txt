#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
]]
find_package(Threads REQUIRED)

set(src
    "fstlog.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/fstlog)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/fstlog)
add_executable(fstlog-stfix ${ALL_FILES})
set_target_properties(fstlog-stfix PROPERTIES
    OUTPUT_NAME "fstlog-v${FSTLOG_VERSION}-stfix" 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(fstlog-stfix PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench CONFIG REQUIRED)
find_package(fstlog CONFIG REQUIRED)

target_link_libraries(fstlog-stfix
	PRIVATE fstlog::fstlog
    PRIVATE logbench::logbench
)
