cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE UTILS_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(UTILS_SOURCES ${UTILS_SOURCES} PARENT_SCOPE)
