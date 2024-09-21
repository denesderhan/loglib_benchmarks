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
add_executable(spdlog ${ALL_FILES})
set_target_properties(spdlog PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(spdlog PRIVATE
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
