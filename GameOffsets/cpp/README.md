# C++ Implementation of GameOffsets Pattern System

This directory contains a C++ implementation of the C# Pattern and StaticOffsetsPatterns functionality from the GameOffsets project.

## Overview

The C++ implementation provides equivalent functionality to the original C# code for memory pattern scanning, including:

- **Pattern class**: Represents a memory pattern with hex byte sequences, wildcards, and skip offsets
- **StaticOffsetsPatterns**: Collection of predefined patterns for game memory scanning
- Support for hex pattern parsing with wildcards (`??` or `?`)
- Auto-calculation of byte skip offsets using the `^` marker
- Pattern validation and debugging through toString() method

## Files

- `Pattern.h` / `Pattern.cpp` - Core Pattern class implementation
- `StaticOffsetsPatterns.h` / `StaticOffsetsPatterns.cpp` - Predefined pattern collections
- `CMakeLists.txt` - Build configuration
- `test_main.cpp` - Test suite demonstrating functionality

## Building

### Prerequisites
- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10 or later

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

This will create:
- `libGameOffsetsPatterns.a` - Static library
- `test_patterns` - Test executable

### Running Tests

```bash
./test_patterns
```

## Usage Example

```cpp
#include "Pattern.h"
#include "StaticOffsetsPatterns.h"

// Create a pattern manually
GameOffsets::Pattern pattern("My Pattern", "48 83 EC ?? 48 8B F1", 5);

// Use predefined patterns
const auto& patterns = GameOffsets::StaticOffsetsPatterns::Patterns;
for (const auto& pattern : patterns) {
    std::cout << pattern.toString() << std::endl;
}
```

## Pattern Format

Patterns support the following syntax:
- Hex bytes: `48`, `0x48`, `0X48` (case insensitive)
- Wildcards: `??`, `?` (represents don't care bytes)
- Skip marker: `^` (auto-calculates bytesToSkip offset)
- Delimiters: Space or comma

### Examples

```cpp
// Basic pattern with manual skip offset
Pattern pattern1("Test", "48 83 EC ?? 48", 3);

// Pattern with auto-calculated skip offset
Pattern pattern2("Test", "48 83 ^ EC ?? 48");  // bytesToSkip = 2

// Mixed hex formats
Pattern pattern3("Test", "0x48, 0x83 EC ?? 48");
```

## Key Features

1. **Hex Parsing**: Flexible parsing of hex strings with various formats
2. **Wildcard Support**: Use `??` or `?` for bytes that should be ignored during matching
3. **Auto Skip Calculation**: Use `^` marker to automatically calculate bytesToSkip offset
4. **Pattern Validation**: Built-in validation and error handling for invalid hex values
5. **Debug Output**: toString() method for pattern debugging and logging

## Predefined Patterns

The implementation includes the following predefined patterns from the original C# code:

1. **Game States** - For finding game state structures
2. **File Root** - For file system root access
3. **AreaChangeCounter** - For tracking area changes
4. **GameWindowScaleValues** - For UI scaling values
5. **Terrain Rotator Helper** - For terrain rotation calculations
6. **Terrain Rotation Selector** - For terrain rotation selection

Each pattern includes detailed comments explaining how it was discovered and its purpose.

## Integration

The C++ library can be integrated into existing C++ projects by:

1. Including the header files
2. Linking against the static library
3. Using the namespace `GameOffsets`

The API is designed to be simple and intuitive, closely matching the original C# interface while following C++ best practices.