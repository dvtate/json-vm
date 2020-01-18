cmake_minimum_required(VERSION 3.15)
project(json_lisp)

set(CMAKE_CXX_STANDARD 17)

add_executable(json_lisp src/test.cpp src/libs/json.hpp src/interpret/value.cpp src/interpret/value.hpp src/interpret/state.cpp src/interpret/state.hpp src/interpret/operators/operator.hpp)