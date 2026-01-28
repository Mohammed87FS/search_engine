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


set(SOURCES
    src/tokenizer.cpp
    src/util.cpp
    src/document_store.cpp
    src/index.cpp
    src/search.cpp
    src/file_scanner.cpp
    src/main_gui.cpp
)


set(HEADERS
    include/tokenizer.hpp
    include/document_store.hpp
    include/index.hpp
    include/search.hpp
    include/file_scanner.hpp
    include/util.hpp
)


add_executable(notesearch_gui ${SOURCES} ${HEADERS})


target_link_libraries(notesearch_gui 
    shell32 
    comdlg32 
    ole32
)


if(MINGW)
    target_link_libraries(notesearch_gui stdc++fs)
    set_target_properties(notesearch_gui PROPERTIES
        LINK_FLAGS "-mwindows"
    )
elseif(MSVC)
    set_target_properties(notesearch_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()


set_target_properties(notesearch_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
