/*
 * test/test.hpp
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

#pragma once
#ifndef H_54E531F7_9154_454B_BEB9_257408429470
#define H_54E531F7_9154_454B_BEB9_257408429470

#include <exception>
#include <vector>
#include <cstring>
#include <cstdio>

namespace test {

struct AssertFailedError {};

void assert_fail (
		const char* filename,
		int line,
		const char* err_msg) {

	const char* basename = filename + strlen(filename);
	while (basename != filename && *basename != '/') {
		basename -= 1;
	}
	basename += 1;

	printf("!! assertion failed:\n\tat: %s:%i\n\t%s\n",
			basename, line, err_msg);
	throw AssertFailedError();
}

struct TestBase {
	const char* name;
	virtual ~TestBase() {}
	TestBase(const char*);
	virtual void do_test() = 0;

	bool run() {
		try {
			do_test();
			return true;
		} catch(const AssertFailedError&) {
		} catch(const std::exception& e) {
			printf("/!\\ Exception: %s\n", e.what());
		} catch(...) {
			printf("/!\\ Unknown exception\n");
		}
		return false;
	}
};

typedef std::vector<TestBase*> test_registry_t;
extern test_registry_t test_registry;

TestBase::TestBase(const char* n): name(n) {
	test_registry.push_back(this);
}

} // namespace test

#define ASSERT(expr) \
	(expr) ? static_cast<void>(0) \
	: ::test::assert_fail( \
			__FILE__, __LINE__, "assertion: " #expr)

#define ASSERT_EQ(a, b) \
	(a == b) ? static_cast<void>(0) \
	: ::test::assert_fail( \
			__FILE__, __LINE__, "err: " #a " != " #b)

#define TEST(name) \
	struct TEST_##name: ::test::TestBase { \
		TEST_##name(): TestBase(#name) {} \
		void do_test(); \
	} TEST_##name; \
	void TEST_##name::do_test()

#endif /* H_GUARD */
