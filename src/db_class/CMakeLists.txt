include_directories(..//include)
# Define library sources
set(LIB_SOURCES
    db_class.cpp
)

# link_directories(${CMAKE_BINARY_DIR}/libs)

#compile into static
add_library(db_class ${LIB_SOURCES})

# target_link_libraries(db_class sqlite)


#add properties and export import lib
set_target_properties(db_class PROPERTIES
  LANGUAGE CXX
  OUTPUT_NAME db_class
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
)