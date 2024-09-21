#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: BSL-1.0
 */
]]
set(src
    "boost.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/boost)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/boost)
add_executable(boost ${ALL_FILES})
set_target_properties(boost PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

target_include_directories(boost PRIVATE
	"./"
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)
find_package(boost_log REQUIRED)

target_link_libraries(boost
	PRIVATE Boost::log
    PRIVATE logbench::logbench
)
