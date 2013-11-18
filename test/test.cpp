
#include <iostream>
#include <stdexcept>
#include "test/test.hpp"

TEST (empty_test) { }

TEST_FAIL_ASSERT (fail_assert) {
	ASSERT(1 == 2);
}

TEST_FAIL_ASSERT (fail_assert_ge) {
	ASSERT_GE(4, 5);
}

TEST_UNCAUGHT_EXCEPTION (uncaught_exception) {
	throw std::runtime_error("some random runtime error");
}

TEST_UNCAUGHT_EXCEPTION (uncaught_exception_int) {
	throw 42;
}

TEST_SEGFAULT (segfault) {
	char* a = 0;
	char b = a[42];
	std::cout << "result: " << b << std::endl;
}

TEST_ABORT (abort) {
	abort();
}

TEST (catch_int) {
	ASSERT_THROW({throw 42;}, int);
}

TEST_FAIL_ASSERT (fail_catch_int) {
	ASSERT_THROW({}, int);
}

TEST_FAIL_ASSERT (fail_no_throw) {
	ASSERT_NO_THROW({throw 42;});
}

TEST (any_throw) {
	ASSERT_ANY_THROW({throw 42;});
}

TEST_FAIL_ASSERT (fail_any_throw) {
	ASSERT_ANY_THROW({});
}
