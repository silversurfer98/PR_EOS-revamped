include_directories(..//include)

set(LIB_SOURCES maths.cpp)

#compile into static
add_library(math ${LIB_SOURCES})

#add properties and export import lib
set_target_properties(math PROPERTIES
  LANGUAGE CXX
  OUTPUT_NAME math
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
)