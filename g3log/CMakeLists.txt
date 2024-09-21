#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
set(src
    "g3log.cpp"
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


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/g3log)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/g3log)
add_executable(g3logb ${ALL_FILES})
set_target_properties(g3logb PROPERTIES
	OUTPUT_NAME "g3log"
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(g3logb PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(g3log REQUIRED)

target_link_libraries(g3logb PRIVATE 
    g3log
    logbench::logbench
)
