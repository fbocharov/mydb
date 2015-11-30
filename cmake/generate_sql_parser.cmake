cmake_minimum_required(VERSION 3.2)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(BISON_INPUT ${CMAKE_SOURCE_DIR}/src/sql/sql.y)
set(BISON_OUTPUT ${CMAKE_SOURCE_DIR}/src/sql/parser.cpp)
set(FLEX_INPUT ${CMAKE_SOURCE_DIR}/src/sql/sql.lex)
set(FLEX_OUTPUT ${CMAKE_SOURCE_DIR}/src/sql/scanner.cpp)

bison_target(SqlParser ${BISON_INPUT} ${BISON_OUTPUT})
flex_target(SqlScanner ${FLEX_INPUT} ${FLEX_OUTPUT})
add_flex_bison_dependency(SqlScanner SqlParser)

#add_executable(Foo
#	${FLEX_SqlParser_OUTPUTS}
#	${BISON_SqlParser_OUTPUTS}
#)
#if(BISON_FOUND)
#	add_custom_command(
#		output ${BISON_OUTPUT}
#		command ${BISON_EXECUTABLE}
#			--defines=${CMAKE_SOURCE_DIR}/src/sql/tokens.h
#			--output=${BISON_OUTPUT}
#			${CMAKE_SOURCE_DIR}/src/sql/sql.y
#		comment "Generating parser.cpp"
#	)
#endif()

#if(FLEX_FOUND)
#	add_custom_command(
#		output ${FLEX_OUTPUT}
#		command ${FLEX_EXECUTABLE}
#			--outfile=${FLEX_OUTPUT}
#			${CMAKE_SOURCE_DIR}/src/sql/sql.lex
#		comment "Generating scanner.cpp"
#	)
#endif()
