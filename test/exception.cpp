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
	backward::raise(std::runtime_error("OmyGAAAD! Exception!"));
}

void a_sub_function() {
	try {
		another_sub_function();
	} catch (...) {
		throw;
	}
}

void a_little_function() {
	try {
		a_sub_function();
	} catch (...) {
		throw;
	}
}

TEST(rethrow) {
	try {
		a_little_function();
	} catch (const std::exception& e) {
		ExceptionHandling::pprint_current_exception();
	}
}

#ifdef BACKWARD_ATLEAST_CXX11

void nested_level3() {
	throw std::runtime_error("OmyGAAAD! Exception!");
}

void nested_level2() {
	try {
		nested_level3();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("level1!"));
	}
}

void nested_level1() {
	try {
		nested_level2();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("and level two"));
	}
}

static int max_nested_level = 0;

void print_nested_ex(std::exception& e, int level=0) {
	std::cout << std::string(level, ' ')
		<< " - exception: " << e.what() << std::endl;
	max_nested_level = std::max(max_nested_level, level);
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception& e) {
		print_nested_ex(e, level + 1);
	}
}

TEST(nested) {
	try {
		nested_level1();
	} catch (std::exception& e) {
		print_nested_ex(e);
	}
	ASSERT_EQ(max_nested_level, 2);
}

void nested_2_level3() {
	backward::raise(std::runtime_error("OmyGAAAD! Exception!"));
}

void nested_2_level2() {
	try {
		nested_2_level3();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("level1!"));
	}
}

void nested_2_level1() {
	try {
		nested_2_level2();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("and level two"));
	}
}

static int max_nested_2_level = 0;

void print_nested_2_ex(std::exception& e, int level=0) {
	std::cout << std::string(level, ' ')
		<< " - exception: " << e.what() << std::endl;
	max_nested_2_level = std::max(max_nested_2_level, level);
	try {
		std::rethrow_if_nested(e);
	} catch (std::exception& e) {
		print_nested_2_ex(e, level + 1);
	}
}

TEST(nested2) {
	try {
		nested_2_level1();
	} catch (std::exception& e) {
		print_nested_2_ex(e);
	}
	ASSERT_EQ(max_nested_2_level, 2);
}

#endif

TEST(not_pprint_current_exception) {
	backward::pprint_current_exception();
}
