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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/fstlog-mt)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/fstlog-mt)
add_executable(fstlog-mt ${ALL_FILES})
set_target_properties(fstlog-mt PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(fstlog-mt PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(fstlog REQUIRED)

target_link_libraries(fstlog-mt
	PRIVATE fstlog::fstlog
    PRIVATE logbench::logbench
)
