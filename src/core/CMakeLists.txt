cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE CORE_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(CORE_SOURCES ${CORE_SOURCES} PARENT_SCOPE)
