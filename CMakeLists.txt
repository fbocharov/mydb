#Top-level cmake-list

cmake_minimum_required(VERSION 3.2)

project(MyDB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -D_DEBUG -Wall -pedantic -Wno-long-long -std=c++1y" CACHE STRING "Debug options." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -fno-omit-frame-pointer -D_NDEBUG -Wall -pedantic -Wno-long-long -std=c++1y" CACHE STRING "Release options." FORCE)

add_subdirectory(src)
add_subdirectory(test)
