find_package(Threads REQUIRED)
set(CMAKE_BUILD_TYPE Release)
# Define library sources
set(LIB_SOURCES
    sqlite3.c
)

#compile into dll
add_library(sqlite SHARED ${LIB_SOURCES})

target_link_libraries(sqlite PRIVATE Threads::Threads)

#add properties and export import lib
set_target_properties(sqlite PROPERTIES
  LANGUAGE C
  OUTPUT_NAME sqlite
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs
  # LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_bin
  WINDOWS_EXPORT_ALL_SYMBOLS TRUE
  DEFINE_SYMBOL MYDLL_EXPORTS
)

if (EXISTS "${CMAKE_BINARY_DIR}/bin/libsqlite.dll")
    message("DLL exsists")
else()
    install(TARGETS sqlite RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
