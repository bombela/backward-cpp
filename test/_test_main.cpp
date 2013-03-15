/*
 * _test_main.cpp
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

#include "test.hpp"
#include <stdio.h>

test::test_registry_t test::test_registry;
using namespace test;

int main()
{
	size_t failed_test_cnt = 0;
	for (test_registry_t::iterator test = test_registry.begin();
			test != test_registry.end(); ++test) {
		printf("-- running test case: %s\n", (*test)->name);
		const bool success = (*test)->run();
		if (not success) {
			printf("\n-- test case failed: %s\n", (*test)->name);
			failed_test_cnt += 1;
		} else {
			printf("\n-- test case success: %s\n", (*test)->name);
		}
	}
	//printf("-- %zi/%zi test(s) case failed\n", failed_test_cnt, test_registry.size());
	return failed_test_cnt != 0;
}
