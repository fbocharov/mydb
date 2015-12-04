cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE SQL_SOURCES "*.c" "*.cpp" "*.h" "*.hpp")
set(SQL_SOURCES ${SQL_SOURCES} PARENT_SCOPE)
