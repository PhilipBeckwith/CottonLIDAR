cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(pclMaker)

find_package(PCL 1.7 REQUIRED)

include_directories( ${Readline_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS})
link_directories( ${Readline_LIBRARY_DIRS} ${PCL_LIBRARY_DIRS})
add_definitions( ${Readline_DEFINITIONS} ${PCL_DEFINITIONS})

add_executable (pclMaker pclMaker.cpp)
target_link_libraries (pclMaker ${PCL_LIBRARIES} readline)

#target_link_libraries (pclMaker ${Readline_LIBRARIES})
