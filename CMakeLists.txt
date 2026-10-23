cmake_minimum_required(VERSION 3.16)

file(READ "VERSION" version_raw)
string(STRIP "${version_raw}" version)

project(YAD-Studio VERSION "${version}" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(YAD-Studio/)
