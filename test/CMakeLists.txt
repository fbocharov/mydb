cmake_minimum_required(VERSION 3.2)

project(mydb_test)

file(GLOB_RECURSE SOURCES "main.cpp" "*Test.cpp")

enable_testing()
find_package(GTest REQUIRED)

if(NOT GTEST_FOUND)
	message(FATAL_ERROR "GTest can't be found.")
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${GTEST_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	${GTEST_LIBRARY}
	${GTEST_MAIN_LIBRARY}
	pthread
	mydb.lib
)
