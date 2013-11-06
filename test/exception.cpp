/*
 * test/exception.cpp
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "backward.hpp"

#include <stdio.h>
#include <stdexcept>
#include <exception>
#include "test/test.hpp"

using namespace backward;

void uncaught_level2() {
	throw std::runtime_error("OmyGAAAD! Exception!");
}

void uncaught_level1() {
	uncaught_level2();
}

TEST_UNCAUGHT_EXCEPTION(uncaught) {
	uncaught_level1();
}

void another_sub_function() {
    //throw std::runtime_error("OmyGAAAD! Exception!");
	backward::raise(std::runtime_error("OmyGAAAD! Exception!"));
}

void a_sub_function() {
	try {
		another_sub_function();
	} catch (...) {
//        ExceptionHandling::pprint_current_exception();
		throw;
	}
}

void a_little_function() {
	try {
		a_sub_function();
	} catch (...) {
//        ExceptionHandling::pprint_current_exception();
		throw;
	}
}

TEST_UNCAUGHT_EXCEPTION(rethrow) {
//    try {
		a_little_function();
//    } catch (const std::exception& e) {
//        ExceptionHandling::pprint_current_exception();
//    }
}

#ifdef BACKWARD_ATLEAST_CXX11

void nested_level2() {
	throw std::runtime_error("OmyGAAAD! Exception!");
}

void nested_level1() {
	try {
		nested_level2();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("vas y prend ca"));
	}
}

void print_ex(std::exception& e, int level=0) {
	std::cout << std::string(level, ' ')
		<< "exception: " << e.what() << std::endl;
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception& e) {
		print_ex(e, 1);
	}
}

TEST(nested) {
	try {
		a_little_function();
	} catch (std::exception& e) {
		print_ex(e);
	}
}

#endif
