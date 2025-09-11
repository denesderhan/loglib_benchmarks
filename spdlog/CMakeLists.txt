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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/spdlog)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/spdlog)
add_executable(spdlogv1 ${ALL_FILES})
set_target_properties(spdlogv1 PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(spdlogv1 PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(spdlogv1
	PRIVATE spdlog::spdlog
    PRIVATE logbench::logbench
)
