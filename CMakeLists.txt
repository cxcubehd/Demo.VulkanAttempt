cmake_minimum_required(VERSION 3.30)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)


# define project
project(lxe LANGUAGES CXX)


# Configuration
add_subdirectory(cmake)


# Asset files
add_subdirectory(assets)


#message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
#message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")