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

TEST(pprint_exception_primitive) {
	try {
		throw 42;
	} catch (...) {
		backward::pprint_current_exception();
	}
}

TEST(pprint_no_base_exception) {
	struct error { };
	try {
		throw error();
	} catch (...) {
		backward::pprint_current_exception();
	}
}

TEST(pprint_exception) {
	try {
		throw std::runtime_error("This is a really useless message.");
	} catch (...) {
		backward::pprint_current_exception();
	}
}

void another_sub_function() {
	throw std::runtime_error("Something bad happened here.");
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
	} catch (...) {
		backward::pprint_current_exception();
	}
}

#ifdef BACKWARD_ATLEAST_CXX11

std::exception_ptr catch_something() {
	try {
		throw std::runtime_error("Something bad happened here.");
	} catch(...) {
		return std::current_exception();
	}
}

TEST(rethrow_exception_ptr) {
	auto eptr = catch_something();
	try {
		std::rethrow_exception(eptr);
	} catch (...) {
		backward::pprint_current_exception();
	}
}

void nested_level4() {
	throw std::runtime_error("Shit's hitting the fan!");
}

void nested_level3() {
	try {
		nested_level4();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("It's flying everywhere!"));
	}
}

void nested_level2() {
	try {
		nested_level3();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("We are deep in it."));
	}
}

void nested_level1() {
	try {
		nested_level2();
	} catch (std::exception& e) {
		std::throw_with_nested(std::runtime_error("What a mess."));
	}
}

TEST(nested) {
	try {
		nested_level1();
	} catch (...) {
		backward::pprint_current_exception();
	}
}

#endif // BACKWARD_ATLEAST_CXX11

void throw_too_far() {
	throw std::runtime_error("Catch me if you can.");
}

TEST_TERMINATE_HANDLER(uncaught) {
	throw_too_far();
}


// ------------------
#ifdef BACKWARD_ATLEAST_CXX11

void nested_2_level3() {
	backward::throw_with_st(std::runtime_error("OmyGAAAD! Exception!"));
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

TEST(nested2) {
	try {
		nested_2_level1();
	} catch (std::exception& e) {
		backward::pprint_current_exception();
	}
}

TEST_TERMINATE_HANDLER(nested3) {
	nested_2_level1();
}

#endif  // BACKWARD_ATLEAST_CXX11

TEST(not_pprint_current_exception) {
	backward::pprint_current_exception();
}

void uncaught2_level2() {
	backward::throw_with_st(std::runtime_error("OmyGAAAD! Exception!"));
}

void uncaught2_level1() {
	try {
		uncaught2_level2();
	} catch (...) {
		std::throw_with_nested(std::runtime_error("nested"));
	}
}

TEST_TERMINATE_HANDLER(uncaught2) {
	uncaught2_level1();
}
