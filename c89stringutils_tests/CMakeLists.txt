cmake_minimum_required(VERSION 3.11)
project(c89stringutils_tests
        VERSION "0.0.1"
        LANGUAGES C)

set(HOMEPAGE_URL "https://github.com/offscale/c89stringutils")
set(DESCRIPTION "Tests for c89stringutils")
set(CMAKE_C_STANDARD 90)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory("test_c89stringutils")
