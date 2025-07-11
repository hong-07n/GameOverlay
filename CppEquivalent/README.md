# C++ Equivalent of GameHelper PatternFinder and SafeMemoryHandle

This directory contains C++ implementations equivalent to the C# `PatternFinder.cs` and `SafeMemoryHandle.cs` from the GameHelper project.

## Files Overview

### Core Classes
- **`Pattern.hpp/cpp`** - C++ equivalent of the Pattern struct for defining byte patterns with wildcards
- **`SafeMemoryHandle.hpp/cpp`** - C++ wrapper for Windows process memory access  
- **`PatternFinder.hpp/cpp`** - C++ pattern scanning functionality with parallel processing

### Usage Example
- **`UsageExample.cpp`** - Complete example showing how to use both classes
- **`CMakeLists.txt`** - Build configuration for CMake

## Features

### PatternFinder
- **Parallel Processing** - Uses C++20 parallel algorithms for fast pattern scanning
- **Memory Efficient** - Reads memory in chunks to avoid large allocations
- **Wildcard Support** - Use `??` or `?` for bytes that should be ignored
- **Flexible Patterns** - Support for hex patterns with automatic offset calculation using `^`

### SafeMemoryHandle  
- **Safe Memory Access** - RAII wrapper around Windows process handles
- **Template Methods** - Generic methods for reading any data type
- **String Support** - Built-in support for ASCII and Unicode strings
- **Array Reading** - Efficient reading of arrays from process memory

### Pattern Format
Patterns use the same format as the C# version:
- Hex bytes: `48 83 EC 20`
- Wildcards: `48 83 ?? 20` or `48 83 ? 20`
- Auto offset: `48 83 EC ^ 20 48 8B` (calculates bytesToSkip automatically)
- Manual offset: Use constructor parameter

## Building

### Requirements
- **C++20 compatible compiler** (Visual Studio 2019+, GCC 10+, Clang 10+)
- **Windows** (uses Windows API for process memory access)
- **CMake 3.16+**

### Build Steps

1. **Using CMake (Recommended)**
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

2. **Using Visual Studio**
```bash
# Generate Visual Studio solution
cmake -G "Visual Studio 16 2019" -A x64 ..
# Open the .sln file in Visual Studio
```

3. **Manual Compilation** (if not using CMake)
```bash
# Compile library
g++ -std=c++20 -c Pattern.cpp SafeMemoryHandle.cpp PatternFinder.cpp

# Compile example
g++ -std=c++20 -o UsageExample UsageExample.cpp Pattern.o SafeMemoryHandle.o PatternFinder.o -lpsapi
```

## Usage Examples

### 1. Basic Memory Reading

```cpp
#include "SafeMemoryHandle.hpp"
#include <iostream>

int main() {
    // Open process by ID
    DWORD processId = 1234; // Replace with actual process ID
    SafeMemoryHandle handle(processId);
    
    if (!handle.IsValid()) {
        std::cerr << "Failed to open process" << std::endl;
        return 1;
    }
    
    // Read a 32-bit integer from address
    uintptr_t address = 0x400000;
    uint32_t value = handle.ReadMemory<uint32_t>(address);
    
    // Read an array of bytes
    auto bytes = handle.ReadMemoryArray<uint8_t>(address, 16);
    
    // Read a string
    std::string text = handle.ReadString(address);
    
    return 0;
}
```

### 2. Pattern Scanning

```cpp
#include "PatternFinder.hpp"
#include "SafeMemoryHandle.hpp"
#include <iostream>

int main() {
    // Create patterns
    std::vector<Pattern> patterns;
    patterns.emplace_back("Game State", "48 83 EC ?? 48 8B F1 33 ED 48 39 2D ^ ?? ?? ?? ??");
    patterns.emplace_back("File Root", "48 ?? ?? ^ ?? ?? ?? ?? 41 ?? ?? ?? 39 ?? ?? ?? ?? ?? 0F 8E");
    
    // Open process
    SafeMemoryHandle handle(processId);
    
    // Get process module info (base address and size)
    uintptr_t baseAddress = 0x400000;  // Replace with actual base
    size_t moduleSize = 0x100000;      // Replace with actual size
    
    try {
        // Find patterns
        auto results = PatternFinder::Find(handle, baseAddress, moduleSize, patterns);
        
        // Use results
        for (const auto& [name, offset] : results) {
            std::cout << name << " found at: 0x" << std::hex << offset << std::endl;
            
            // Read data from found offset
            uint32_t value = handle.ReadMemory<uint32_t>(offset);
            std::cout << "Value at offset: " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Pattern search failed: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### 3. Creating Custom Patterns

```cpp
#include "Pattern.hpp"

// Pattern with automatic offset calculation
Pattern pattern1("My Pattern", "48 83 EC ?? 48 8B ^ F1 33 ED");

// Pattern with manual offset
Pattern pattern2("Manual Pattern", "48 83 EC ?? 48 8B F1", 5);

// Pattern with wildcards
Pattern pattern3("Wildcard Pattern", "4D 5A ?? ?? ?? ?? ?? ?? 50 45");

// Print pattern info
std::cout << pattern1.toString() << std::endl;
```

## Key Differences from C# Version

### Similarities
- **Same pattern format** - Identical hex string format with wildcards and ^ marker
- **Same optimization strategy** - Reads memory in chunks, parallel processing
- **Same search algorithm** - Bidirectional pattern matching for efficiency

### Differences
- **C++20 parallel algorithms** instead of .NET Parallel.For
- **RAII memory management** instead of SafeHandle inheritance
- **Template-based generics** instead of C# generics
- **Standard containers** (std::vector, std::unordered_map) instead of .NET collections

## Error Handling

The C++ implementation uses exceptions for error handling:

```cpp
try {
    auto results = PatternFinder::Find(handle, baseAddress, size, patterns);
    // Use results...
} catch (const std::runtime_error& e) {
    if (std::string(e.what()).find("Couldn't find") != std::string::npos) {
        // Some patterns were not found
        std::cerr << "Missing patterns: " << e.what() << std::endl;
    } else if (std::string(e.what()).find("non-unique") != std::string::npos) {
        // Pattern matched multiple locations  
        std::cerr << "Ambiguous patterns: " << e.what() << std::endl;
    }
}
```

## Performance Notes

- **Parallel Processing**: Utilizes all available CPU cores for pattern scanning
- **Memory Efficient**: Reads process memory in 84KB chunks to avoid large allocations
- **Optimized Matching**: Uses bidirectional pattern matching for better cache locality
- **Early Exit**: Stops scanning once all patterns are found

## Platform Support

Currently supports **Windows only** due to the use of Windows API functions:
- `OpenProcess`
- `ReadProcessMemory` 
- `CloseHandle`

For cross-platform support, the SafeMemoryHandle class would need platform-specific implementations.

## License

This C++ implementation follows the same license as the original C# code in the GameOverlay project.