project(backward-package-test)
cmake_minimum_required(VERSION 3.14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE ${CONAN_TARGETS})
