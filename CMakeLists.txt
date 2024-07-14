cmake_minimum_required(VERSION 3.29)
project(BoostExamples)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

include(${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake)


# Add the executable
add_executable(BoostExamples main.cpp)


add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(chatapp)
