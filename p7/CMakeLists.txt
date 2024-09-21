#[[
/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
]]
find_package(Threads REQUIRED)
find_package(logbench REQUIRED)

set(src
    "p7.cpp"
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}/p7)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${OUT_DIR}/p7)
add_executable(p7b ${ALL_FILES})
set_target_properties(p7b PROPERTIES 
	OUTPUT_NAME "p7"
	DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)


target_include_directories(p7b PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}"
)
find_file(P7_INC_F
		P7/P7_Client.h
		PATHS "$ENV{P7_INCLUDE_PATH}" ${CMAKE_CURRENT_LIST_DIR}
	)
if(NOT P7_INC_F)
	message(ERROR " Baical P7 include files not detected. Copy includes into loglib_benchmarks/p7/P7 or set P7_INCLUDE_PATH environment variable.")
else()
	get_filename_component(TEMP_DIR "${P7_INC_F}" DIRECTORY)
	cmake_path(GET TEMP_DIR PARENT_PATH P7_INC)
	target_include_directories(p7b PRIVATE
		"${P7_INC}"
	)
endif()

find_library(P7_LIBRARY 
	NAMES p7 P7x64
	PATHS "$ENV{P7_LIB_PATH}"
)
if(NOT P7_LIBRARY)
	message(ERROR "Baical P7 library not found, set environment variable P7_LIB_PATH!")
endif()

target_link_libraries(p7b PRIVATE
	"${P7_LIBRARY}"
	logbench::logbench
)
