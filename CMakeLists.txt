cmake_minimum_required(VERSION 3.15)
project(NoteSearch VERSION 1.0.0 LANGUAGES CXX)


if(NOT WIN32)
    message(FATAL_ERROR "This project is Windows-only")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()


include_directories(${CMAKE_SOURCE_DIR}/include)


# alles ausser den main dateien, wird von GUI, CLI und tests benutzt
set(CORE_SOURCES
    src/tokenizer.cpp
    src/util.cpp
    src/document_store.cpp
    src/index.cpp
    src/search.cpp
    src/file_scanner.cpp
)


set(HEADERS
    include/tokenizer.hpp
    include/document_store.hpp
    include/index.hpp
    include/search.hpp
    include/file_scanner.hpp
    include/util.hpp
)


add_library(notesearch_core STATIC ${CORE_SOURCES} ${HEADERS})

if(MINGW)
    target_link_libraries(notesearch_core PUBLIC stdc++fs)
endif()


# GUI
add_executable(notesearch_gui src/main_gui.cpp)

target_link_libraries(notesearch_gui
    notesearch_core
    shell32
    comdlg32
    ole32
)

if(MINGW)
    set_target_properties(notesearch_gui PROPERTIES
        LINK_FLAGS "-mwindows"
    )
elseif(MSVC)
    set_target_properties(notesearch_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()


# CLI
add_executable(notesearch src/main.cpp)
target_link_libraries(notesearch notesearch_core)


# tests, laufen im repo ordner wegen notes_examples
enable_testing()
add_executable(notesearch_tests tests/test_search.cpp)
target_link_libraries(notesearch_tests notesearch_core)
add_test(NAME search_tests COMMAND notesearch_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


set_target_properties(notesearch_gui notesearch notesearch_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
