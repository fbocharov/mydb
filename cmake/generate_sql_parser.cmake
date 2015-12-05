cmake_minimum_required(VERSION 3.2)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(BISON_INPUT ${CMAKE_SOURCE_DIR}/src/sql/sql.ypp)
set(BISON_OUTPUT ${CMAKE_SOURCE_DIR}/src/sql/SQLParserInternals.cpp)
set(FLEX_INPUT ${CMAKE_SOURCE_DIR}/src/sql/sql.lex)
set(FLEX_OUTPUT ${CMAKE_SOURCE_DIR}/src/sql/SQLScannerInternals.cpp)
set(FLEX_HEADER ${CMAKE_SOURCE_DIR}/src/sql/SQLScannerInternals.hpp)

if(BISON_FOUND)
	add_custom_command(OUTPUT ${BISON_OUTPUT}
		COMMAND ${BISON_EXECUTABLE}
			--defines=${CMAKE_SOURCE_DIR}/src/sql/SQLParserInternals.hpp
			--output=${BISON_OUTPUT}
			${BISON_INPUT}
		COMMENT "Generating parser."
	)
endif()

if(FLEX_FOUND)
	add_custom_command(OUTPUT ${FLEX_OUTPUT}
		COMMAND ${FLEX_EXECUTABLE}
			--outfile=${FLEX_OUTPUT}
			--header-file=${FLEX_HEADER}
			${FLEX_INPUT}
		COMMENT "Generating scanner."
	)
endif()

if(BISON_FOUND AND FLEX_FOUND)
	add_custom_target(SQLGen ALL
		DEPENDS ${FLEX_OUTPUT} ${BISON_OUTPUT}
	)
endif()
