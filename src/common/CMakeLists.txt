cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE COMMON_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
