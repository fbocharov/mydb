cmake_minimum_required(VERSION 2.8)

project(mydb)

file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.h" "*.hpp")

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${Boost_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
)
