cmake_minimum_required(VERSION 3.16)
project(GameOverlayCppEquivalent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable parallel algorithms support
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -ltbb")
endif()

# Create library for the core classes
add_library(GameOverlayCore
    Pattern.cpp
    Pattern.hpp
    SafeMemoryHandle.cpp
    SafeMemoryHandle.hpp
    PatternFinder.cpp
    PatternFinder.hpp
)

# Create executable for usage example
add_executable(UsageExample
    UsageExample.cpp
)

# Link the library to the executable
target_link_libraries(UsageExample GameOverlayCore)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(GameOverlayCore psapi)
    target_link_libraries(UsageExample psapi)
endif()

# Compiler-specific settings
if(MSVC)
    # Enable parallel algorithms on MSVC
    target_compile_definitions(GameOverlayCore PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_definitions(UsageExample PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    
    # Enable parallel execution policy
    target_compile_options(GameOverlayCore PRIVATE /permissive-)
    target_compile_options(UsageExample PRIVATE /permissive-)
else()
    # For GCC/Clang, link TBB for parallel algorithms
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TBB REQUIRED tbb)
    target_link_libraries(GameOverlayCore ${TBB_LIBRARIES})
    target_link_libraries(UsageExample ${TBB_LIBRARIES})
endif()

# Set output directories
set_target_properties(GameOverlayCore PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

set_target_properties(UsageExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)