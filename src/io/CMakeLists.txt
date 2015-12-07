cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE IO_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(IO_SOURCES ${IO_SOURCES} PARENT_SCOPE)
