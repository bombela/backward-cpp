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

cmake_minimum_required(VERSION 2.8)
project(backward CXX)

###############################################################################
# COMPILER FLAGS
###############################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
enable_testing()

###############################################################################
# CONFIGS
###############################################################################

set(STACK_DETAILS_BACKTRACE_SYMBOL TRUE CACHE BOOL "Use backtrace symbol")
set(STACK_DETAILS_DW FALSE CACHE BOOL "Use libdw")
set(STACK_DETAILS_BFD FALSE CACHE BOOL "Use libbfd")

set(STACK_WALKING_UNWIND TRUE CACHE BOOL "Use unwind for stack walking")
set(STACK_WALKING_BACKTRACE FALSE CACHE BOOL "Use backtrace for stack walking")

macro(map_definitions var_prefix define_prefix)
	foreach(def ${ARGV})
		if (${${var_prefix}${def}})
			LIST(APPEND BACKWARD_DEFINITIONS "${define_prefix}${def}=1")
		endif()
	endforeach()
endmacro()

map_definitions("STACK_DETAILS_" "BACKWARD_HAS_" BACKTRACE_SYMBOL DW BFD)
map_definitions("STACK_WALKING_" "BACKWARD_HAS_" UNWIND BACKTRACE)

foreach(def ${BACKWARD_DEFINITIONS})
	message(STATUS "${def}")
endforeach()

###############################################################################
# TESTS
###############################################################################

include_directories(${CMAKE_SOURCE_DIR})

add_library(test_main SHARED test/_test_main.cpp)

macro(backward_add_test src src_dep)
	get_filename_component(name ${src} NAME_WE)
	set(test_name "test_${name}")

	add_executable(${test_name} ${src} ${src_dep})

	set_target_properties(${test_name} PROPERTIES
		COMPILE_DEFINITIONS "${BACKWARD_DEFINITIONS}")

	target_link_libraries(${test_name} dw bfd dl test_main)
	add_test(${test_name} ${test_name})
endmacro()

# Tests without backward.cpp
set(TESTS
	compile
	minitrace
	smalltrace
	simplerecursive
	fib
	segfault
	invalidread
	suicide
	divbyzero
	select_signals
	)

foreach(test ${TESTS})
	backward_add_test(test/${test}.cpp "")
endforeach()

# Tests with backward.cpp
set(TESTS
	invalidread2
	)

foreach(test ${TESTS})
	backward_add_test(test/${test}.cpp backward.cpp)
endforeach()
