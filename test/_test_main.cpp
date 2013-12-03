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
#include <cstdio>
#include <map>
#include <cstdlib>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>

test::test_registry_t test::test_registry;
using namespace test;

const char* TestStatus_str(TestStatus e) {
	switch (e) {
		case SUCCESS: return "SUCCESS";
		case FAILED: return "FAILED";
		case ASSERT_FAIL: return "ASSERT_FAIL";
		case EXCEPTION_UNCAUGHT: return "EXCEPTION_UNCAUGHT";
		case SIGNAL_UNCAUGHT: return "SIGNAL_UNCAUGHT";
		case SIGNAL_SEGFAULT: return "SIGNAL_SEGFAULT";
		case SIGNAL_ABORT: return "SIGNAL_ABORT";
		case SIGNAL_DIVZERO: return "SIGNAL_DIVZERO";
	};
	abort();
}

bool run_test(TestBase& test) {
	printf("-- running test case: %s\n", test.name);

	fflush(stdout);
	pid_t child_pid = fork();
	if (child_pid == 0) {
		exit(static_cast<int>(test.run()));
	}
	if (child_pid == -1) {
		error(EXIT_FAILURE, 0, "unable to fork");
	}

	int child_status = 0;
	waitpid(child_pid, &child_status, 0);

	test::TestStatus status;

	if (WIFEXITED(child_status)) {
		int exit_status = WEXITSTATUS(child_status);
		if (exit_status & ~test::STATUS_MASK) {
			status = test::FAILED;
		} else {
			status = static_cast<test::TestStatus>(exit_status);
		}
	} else if (WIFSIGNALED(child_status)) {
		const int signum = WTERMSIG(child_status);
		printf("!! signal (%d) %s\n", signum, strsignal(signum));
		switch (signum) {
			case SIGABRT:
				status = test::SIGNAL_ABORT; break;
			case SIGSEGV:
			case SIGBUS:
				status = test::SIGNAL_SEGFAULT; break;
			case SIGFPE:
				status = test::SIGNAL_DIVZERO; break;
			default:
				status = test::SIGNAL_UNCAUGHT;
		}
	} else {
		status = test::SUCCESS;
	}


	bool success = true;

	if (test.expected_status == test::FAILED) {
		success = (status & test::FAILED);
	} else if (test.expected_status == test::SIGNAL_UNCAUGHT) {
		success = (status & test::SIGNAL_UNCAUGHT);
	} else {
		success = (status == test.expected_status);
	}

	if (success) {
		printf("-- test case success: %s\n", test.name);
	} else {
		printf("** status(%s) != expected(%s), test case failed: %s\n",
				TestStatus_str(status), TestStatus_str(test.expected_status),
				test.name);
	}
	return success;
}

int main(int argc, const char* const argv[]) {

	size_t success_cnt = 0;
	size_t total_cnt = 0;
	for (test_registry_t::iterator it = test_registry.begin();
			it != test_registry.end(); ++it) {
		TestBase& test = **it;

		bool consider_test = (argc <= 1);
		for (int i = 1; i < argc; ++i) {
			if (strcasecmp(argv[i], test.name) == 0) {
				consider_test = true;
				break;
			}
		}
		if (not consider_test) {
			continue;
		}

		total_cnt += 1;
		if (run_test(test)) {
			success_cnt += 1;
		}
	}
	printf("-- tests passing: %lu/%lu", success_cnt, total_cnt);
	if (total_cnt) {
		printf(" (%lu%%)\n", success_cnt * 100 / total_cnt);
	} else {
		printf("\n");
	}
	return (success_cnt == total_cnt) ? EXIT_SUCCESS : EXIT_FAILURE;
}
