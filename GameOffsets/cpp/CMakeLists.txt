cmake_minimum_required(VERSION 3.10)
project(GameOffsetsPatterns VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(GameOffsetsPatterns 
    Pattern.cpp
    StaticOffsetsPatterns.cpp
)

# Include directories
target_include_directories(GameOffsetsPatterns PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Compiler-specific options
if(MSVC)
    target_compile_options(GameOffsetsPatterns PRIVATE /W4)
else()
    target_compile_options(GameOffsetsPatterns PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional: Create a test executable
option(BUILD_TESTS "Build test executable" ON)

if(BUILD_TESTS)
    add_executable(test_patterns test_main.cpp)
    target_link_libraries(test_patterns GameOffsetsPatterns)
endif()

# Optional: Install targets
install(TARGETS GameOffsetsPatterns
    EXPORT GameOffsetsPatternsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES Pattern.h StaticOffsetsPatterns.h
    DESTINATION include
)