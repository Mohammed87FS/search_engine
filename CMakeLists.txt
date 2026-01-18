cmake_minimum_required(VERSION 3.15)
project(NoteSearch VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/tokenizer.cpp
    src/util.cpp
    src/document_store.cpp
    src/index.cpp
    src/search.cpp
    src/file_scanner.cpp
    src/main.cpp
)

# Header files
set(HEADERS
    include/tokenizer.hpp
    include/document_store.hpp
    include/index.hpp
    include/search.hpp
    include/file_scanner.hpp
    include/util.hpp
)

# Create executable
add_executable(notesearch ${SOURCES} ${HEADERS})

# Set output directory
set_target_properties(notesearch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    # Linux-specific
    target_link_libraries(notesearch pthread)
elseif(MINGW)
    # MinGW needs filesystem library
    target_link_libraries(notesearch stdc++fs)
endif()

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
