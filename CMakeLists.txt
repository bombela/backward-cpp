#
# CMakeLists.txt
# Copyright 2013 Google Inc. All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 2.8.8)
project(backward CXX)

###############################################################################
# OPTIONS
###############################################################################

option(BACKWARD_TESTS "Compile tests" ON)
option(BACKWARD_ENABLE_ONLY_IN_DEBUG "Enable backward only if build type is Debug/RelWithDebInfo" OFF)

###############################################################################
# COMPILER FLAGS
###############################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

enable_testing()

###############################################################################
# CONFIGS
###############################################################################

set(STACK_DETAILS_BACKTRACE_SYMBOL FALSE CACHE BOOL "Use backtrace symbol")
set(STACK_DETAILS_DW FALSE CACHE BOOL "Use libdw")
set(STACK_DETAILS_BFD FALSE CACHE BOOL "Use libbfd")

set(STACK_WALKING_UNWIND TRUE CACHE BOOL "Use unwind for stack walking")
set(STACK_WALKING_BACKTRACE FALSE CACHE BOOL "Use backtrace for stack walking")

find_library(BACKWARD_LIBDW_PATH dw)
find_library(BACKWARD_LIBBFD_PATH bfd)
if(NOT BACKWARD_LIBDW_PATH STREQUAL "BACKWARD_LIBDW_PATH-NOTFOUND")
	set(STACK_DETAILS_DW TRUE)
	list(APPEND backward_LIBRARIES ${BACKWARD_LIBDW_PATH})
elseif(NOT BACKWARD_LIBBFD_PATH STREQUAL "BACKWARD_LIBBFD_PATH-NOTFOUND")
	set(STACK_DETAILS_BFD TRUE)
	list(APPEND backward_LIBRARIES ${BACKWARD_LIBBFD_PATH})
else()
	set(STACK_DETAILS_BACKTRACE_SYMBOL TRUE)
	message(INFO "Could not find advanced symbol information library (libdw/libbfd), falling back to backtrace")
endif()

find_library(BACKWARD_LIBUNWIND_PATH unwind)
if(NOT BACKWARD_LIBUNWIND_PATH STREQUAL "BACKWARD_LIBUNWIND_PATH-NOTFOUND")
	set(STACK_WALKING_UNWIND TRUE)
	list(APPEND backward_LIBRARIES ${BACKWARD_LIBUNWIND_PATH})
else()
	set(STACK_WALKING_BACKTRACE TRUE)
	message(INFO "Could not find stack backtrace library (libunwind), falling back to backtrace")
endif()

macro(map_definitions var_prefix define_prefix)
	foreach(def ${ARGV})
		if (${${var_prefix}${def}})
			LIST(APPEND backward_DEFINITIONS "${define_prefix}${def}=1")
		endif()
	endforeach()
endmacro()

map_definitions("STACK_DETAILS_" "BACKWARD_HAS_" BACKTRACE_SYMBOL DW BFD)
map_definitions("STACK_WALKING_" "BACKWARD_HAS_" UNWIND BACKTRACE)

foreach(def ${BACKWARD_DEFINITIONS})
	message(STATUS "${def}")
endforeach()

add_library(enable_backward OBJECT backward.cpp)
target_compile_definitions(enable_backward PRIVATE ${backward_DEFINITIONS})

if(BACKWARD_ENABLE_ONLY_IN_DEBUG)
	set(backward_ENABLE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:$<TARGET_OBJECTS:enable_backward> CACHE STRING
	"object to enable automatic backward processing (only in Debug/RelWithDebInfo)")
else()
	set(backward_ENABLE $<TARGET_OBJECTS:enable_backward> CACHE STRING
	"object to enable automatic backward processing")
endif()
set(backward_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "backward include directory")
set(backward_DEFINITIONS ${backward_DEFINITIONS} CACHE STRING "backward feature definitions")
set(backward_LIBRARIES ${backward_LIBRARIES} CACHE STRING "backward libraries")
macro(add_backward target)
	target_include_directories(${target} PRIVATE ${backward_INCLUDE_DIR})
	set_property(TARGET ${target} APPEND PROPERTY COMPILE_DEFINITIONS ${backward_DEFINITIONS})
	set_property(TARGET ${target} APPEND PROPERTY LINK_LIBRARIES ${backward_LIBRARIES})
endmacro()

###############################################################################
# TESTS
###############################################################################

if(BACKWARD_TESTS)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})

	add_library(test_main SHARED test/_test_main.cpp)

	macro(backward_add_test src src_dep)
		get_filename_component(name ${src} NAME_WE)
		set(test_name "test_${name}")

		add_executable(${test_name} ${src} ${src_dep})

		set_target_properties(${test_name} PROPERTIES
			COMPILE_DEFINITIONS "${BACKWARD_DEFINITIONS}")

		target_link_libraries(${test_name} dw bfd dl test_main)
		add_test(NAME ${name} COMMAND ${test_name})
	endmacro()

	# Tests without backward.cpp
	set(TESTS
		test
		stacktrace
		rectrace
		select_signals
		)

	foreach(test ${TESTS})
		backward_add_test(test/${test}.cpp "")
	endforeach()

	# Tests with backward.cpp
	set(TESTS
		suicide
		)

	foreach(test ${TESTS})
		backward_add_test(test/${test}.cpp backward.cpp)
	endforeach()
endif()
