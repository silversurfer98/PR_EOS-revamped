include_directories(..//include)
# Define library sources
set(LIB_SOURCES
    pr_eos_class.cpp
)

link_directories(${CMAKE_BINARY_DIR}/libs)

#compile into static
add_library(pr_eos_class ${LIB_SOURCES})

target_link_libraries(pr_eos_class db_class math)


#add properties and export import lib
set_target_properties(pr_eos_class PROPERTIES
  LANGUAGE CXX
  OUTPUT_NAME pr_eos_class
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
)