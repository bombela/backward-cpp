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
# TESTS
###############################################################################

include_directories(${CMAKE_SOURCE_DIR})

add_library(test_main SHARED test/_test_main.cpp)

macro(backward_add_test src src_dep definition)
	get_filename_component(name ${src} NAME_WE)
	set(test_name "test_${name}")

	add_executable(${test_name} ${src} ${src_dep})

	set(_definition ${definition})
	if(_definition)
		set_target_properties(${test_name}
			PROPERTIES COMPILE_DEFINITIONS ${definition})
	endif()

	target_link_libraries(${test_name} dw bfd dl test_main)
	add_test(${test_name} ${test_name})
endmacro()

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
	)

foreach(test ${TESTS})
	backward_add_test(test/${test}.cpp "" "")
endforeach()

set(TESTS
	invalidread2
	)

set(some_definition "BACKWARD_HAS_DW=1")
#set(some_definition "BACKWARD_NOTHING")
foreach(test ${TESTS})
	backward_add_test(test/${test}.cpp backward.cpp ${some_definition})
endforeach()
