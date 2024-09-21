#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
]]
find_package(Threads REQUIRED)

set(src
    "quill.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/quill)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/quill)
add_executable(quill ${ALL_FILES})
set_target_properties(quill PROPERTIES 
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

find_path(QUILL_INCLUDE_DIRS "quill/Backend.h")
target_include_directories(quill PRIVATE
	"./"
	${QUILL_INCLUDE_DIRS}
)

################################################################################
# Dependencies
################################################################################

find_package(logbench REQUIRED)

target_link_libraries(quill
    PRIVATE logbench::logbench
)
