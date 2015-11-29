cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE BACKEND_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(BACKEND_SOURCES ${BACKEND_SOURCES} PARENT_SCOPE)
