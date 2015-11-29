cmake_minimum_required(VERSION 3.2)

project(mydb)

add_subdirectory(backend)
add_subdirectory(core)
add_subdirectory(sql)
add_subdirectory(utils)

set(LIB_SOURCES
	${BACKEND_SOURCES}
	${CORE_SOURCES}
	${SQL_SOURCES}
	${UTILS_SOURCES}
)
set(BIN_SOURCES
	${LIB_SOURCES}
	main.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${Boost_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${BIN_SOURCES})
add_library(${PROJECT_NAME}.lib STATIC ${LIB_SOURCES})

target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	-lstdc++fs
)
