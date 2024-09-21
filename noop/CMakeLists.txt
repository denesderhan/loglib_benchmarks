#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
find_package(Threads REQUIRED)

set(src
    "noop.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/noop)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/noop)
add_executable(noop ${ALL_FILES})
set_target_properties(noop PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(noop PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)

target_link_libraries(noop
    PRIVATE logbench::logbench
)
