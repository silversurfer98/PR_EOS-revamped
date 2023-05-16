# cmake . -G "MinGW Makefiles" -B ./build

cmake_minimum_required(VERSION 3.0)
project(newton_raphson)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCES nr.cpp)

#compile into static
add_library(nr ${LIB_SOURCES})

#add properties and export import lib
set_target_properties(nr PROPERTIES
  LANGUAGE CXX
  OUTPUT_NAME pr_eos_class
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
)

link_directories(${CMAKE_BINARY_DIR}/libs)

set(SOURCE_FILES main.cpp)
set(LINK_FILES nr)
add_executable(nr_test ${SOURCE_FILES})

target_link_libraries(nr_test ${LINK_FILES})
install(TARGETS nr_test RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)