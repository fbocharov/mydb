cmake_minimum_required(VERSION 3.2)

project(mydb)

add_subdirectory(backend)
add_subdirectory(core)
add_subdirectory(sql)
add_subdirectory(utils)

include(generate_sql_parser)

set(LIB_SOURCES
	${BACKEND_SOURCES}
	${CORE_SOURCES}
	${SQL_SOURCES}
	${UTILS_SOURCES}
	${FLEX_OUTPUT}
	${BISON_OUTPUT}
)
set(BIN_SOURCES
	${LIB_SOURCES}
	main.cpp
)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME}
	${BIN_SOURCES}
)

set(LIBRARY_NAME ${PROJECT_NAME}.lib)

add_library(${LIBRARY_NAME} STATIC ${LIB_SOURCES})

add_dependencies(${PROJECT_NAME} SQLGen)
add_dependencies(${LIBRARY_NAME} SQLGen)

target_link_libraries(${PROJECT_NAME}
	-lstdc++fs
)
