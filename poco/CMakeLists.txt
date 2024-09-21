#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: BSL-1.0
 */
]]
find_package(Threads REQUIRED)

set(src
    "poco.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/poco)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/poco)
add_executable(poco ${ALL_FILES})
set_target_properties(poco PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(poco PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation)

target_link_libraries(poco
	PRIVATE Poco::Foundation
    PRIVATE logbench::logbench
)
