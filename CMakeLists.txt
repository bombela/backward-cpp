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

cmake_minimum_required(VERSION 3.0)
project(backward CXX)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

include(BackwardConfig.cmake)

###############################################################################
# OPTIONS
###############################################################################

option(BACKWARD_SHARED "Build backward as a shared library" OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT DEFINED BACKWARD_TESTS)
	# If this is a top level CMake project, we most lixely want the tests
	set(BACKWARD_TESTS ON CACHE BOOL "Enable tests")
else()
	set(BACKWARD_TESTS OFF CACHE BOOL "Enable tests")
endif()

###############################################################################
# COMPILER FLAGS
###############################################################################

# check if compiler is nvcc or nvcc_wrapper
set(COMPILER_IS_NVCC false)
get_filename_component(COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME)
if (COMPILER_NAME MATCHES "^nvcc")
  set(COMPILER_IS_NVCC true)
endif()

if (DEFINED ENV{OMPI_CXX} OR DEFINED ENV{MPICH_CXX})
   if ( ($ENV{OMPI_CXX} MATCHES "nvcc") OR ($ENV{MPICH_CXX} MATCHES "nvcc") )
     set(COMPILER_IS_NVCC true)
   endif()
endif()

# set CXX standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 11)
if (${COMPILER_IS_NVCC})
  # GNU CXX extensions are not supported by nvcc
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	if (NOT ${COMPILER_IS_NVCC})
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

###############################################################################
# BACKWARD OBJECT
###############################################################################

add_library(backward_object OBJECT backward.cpp)
target_compile_definitions(backward_object PRIVATE ${BACKWARD_DEFINITIONS})
target_include_directories(backward_object PRIVATE ${BACKWARD_INCLUDE_DIRS})
set(BACKWARD_ENABLE $<TARGET_OBJECTS:backward_object> CACHE STRING
	"Link with this object to setup backward automatically")


###############################################################################
# BACKWARD LIBRARY (Includes backward.cpp)
###############################################################################

set(libtype STATIC)
if(BACKWARD_SHARED)
    set(libtype SHARED)
endif()
add_library(backward ${libtype} backward.cpp)
target_compile_definitions(backward PUBLIC ${BACKWARD_DEFINITIONS})
target_include_directories(backward PUBLIC ${BACKWARD_INCLUDE_DIRS})
if(BACKWARD_HAS_EXTERNAL_LIBRARIES)
    set_target_properties(backward PROPERTIES
                                   INTERFACE_LINK_LIBRARIES "${BACKWARD_LIBRARIES}")
endif()
add_library(Backward::Backward ALIAS backward)

###############################################################################
# TESTS
###############################################################################

if(BACKWARD_TESTS)
	enable_testing()

	add_library(test_main OBJECT test/_test_main.cpp)

	macro(backward_add_test src)
		get_filename_component(name ${src} NAME_WE)
		set(test_name "test_${name}")

		add_executable(${test_name} ${src} ${ARGN} $<TARGET_OBJECTS:test_main>)

		target_link_libraries(${test_name} PRIVATE Backward::Backward)

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
		backward_add_test(test/${test}.cpp)
	endforeach()

	# Tests with backward.cpp
	set(TESTS
		suicide
		)

	foreach(test ${TESTS})
		backward_add_test(test/${test}.cpp ${BACKWARD_ENABLE})
	endforeach()
endif()

install(
    FILES "backward.hpp"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    FILES "BackwardConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/backward
)
