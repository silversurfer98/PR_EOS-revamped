# cmake ./src -G "MinGW Makefiles" -B ../build

cmake_minimum_required(VERSION 3.0)
project(DB_access)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (EXISTS "${CMAKE_BINARY_DIR}/bin/libsqlite.dll")
    message("DLL exsists so not compiling it again")
else()
    add_subdirectory(sqlite)
endif()


add_subdirectory(db_class)

include_directories(include)

# Copy the asset file to the binary directory
configure_file(..//assets//props.db ${CMAKE_BINARY_DIR}/bin/props.db COPYONLY)


link_directories(${CMAKE_BINARY_DIR}/libs)

set(SOURCE_FILES main.cpp)
add_executable(final ${SOURCE_FILES})

target_link_libraries(final sqlite db_class)
install(TARGETS final RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)