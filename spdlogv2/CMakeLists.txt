#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
find_package(Threads REQUIRED)

set(src
    "spdlog.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/spdlogv2)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/spdlogv2)
add_executable(spdlogv2 ${ALL_FILES})
set_target_properties(spdlogv2 PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(spdlogv2 PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(spdlog
	PRIVATE spdlog::spdlog
    PRIVATE logbench::logbench
)
