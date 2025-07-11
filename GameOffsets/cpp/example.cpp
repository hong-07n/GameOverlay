#include "Pattern.h"
#include "StaticOffsetsPatterns.h"
#include <iostream>

/**
 * Example usage of the C++ GameOffsets Pattern system
 * This demonstrates how to use both custom patterns and predefined patterns
 */

int main() {
    std::cout << "GameOffsets C++ Pattern Example\n";
    std::cout << "================================\n\n";

    // Example 1: Creating custom patterns
    std::cout << "1. Creating Custom Patterns:\n";
    
    // Pattern with manual bytesToSkip
    GameOffsets::Pattern customPattern1(
        "Custom Pattern 1", 
        "48 8B 05 ?? ?? ?? ?? 85 C0", 
        3
    );
    std::cout << "   " << customPattern1.toString() << "\n";
    
    // Pattern with auto-calculated bytesToSkip using '^' marker
    GameOffsets::Pattern customPattern2(
        "Custom Pattern 2", 
        "48 8B 05 ^ ?? ?? ?? ?? 85 C0"
    );
    std::cout << "   " << customPattern2.toString() << "\n\n";

    // Example 2: Using predefined patterns
    std::cout << "2. Predefined Patterns:\n";
    const auto& patterns = GameOffsets::StaticOffsetsPatterns::Patterns;
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        const auto& pattern = patterns[i];
        std::cout << "   [" << i << "] " << pattern.name << "\n";
        std::cout << "       BytesToSkip: " << pattern.bytesToSkip << "\n";
        std::cout << "       Pattern Size: " << pattern.data.size() << " bytes\n";
        
        // Show first few bytes as example
        std::cout << "       First bytes: ";
        size_t showCount = std::min(pattern.data.size(), size_t(8));
        for (size_t j = 0; j < showCount; ++j) {
            if (pattern.mask[j]) {
                std::cout << "0x" << std::hex << std::uppercase 
                         << static_cast<int>(pattern.data[j]) << " ";
            } else {
                std::cout << "?? ";
            }
        }
        if (pattern.data.size() > showCount) {
            std::cout << "...";
        }
        std::cout << std::dec << "\n\n";
    }

    // Example 3: Pattern analysis
    std::cout << "3. Pattern Analysis:\n";
    const auto& gameStates = patterns[0]; // Game States pattern
    
    std::cout << "   Analyzing: " << gameStates.name << "\n";
    std::cout << "   Total bytes: " << gameStates.data.size() << "\n";
    
    size_t wildcardCount = 0;
    for (bool maskValue : gameStates.mask) {
        if (!maskValue) wildcardCount++;
    }
    
    std::cout << "   Wildcard bytes: " << wildcardCount << "\n";
    std::cout << "   Fixed bytes: " << (gameStates.data.size() - wildcardCount) << "\n";
    std::cout << "   Specificity: " 
              << (100.0 * (gameStates.data.size() - wildcardCount) / gameStates.data.size()) 
              << "%\n\n";

    // Example 4: Error handling
    std::cout << "4. Error Handling Example:\n";
    try {
        // This will throw an exception due to invalid hex
        GameOffsets::Pattern invalidPattern("Invalid", "48 XY 05");
    } catch (const std::exception& e) {
        std::cout << "   Caught expected error: " << e.what() << "\n";
    }

    std::cout << "\nExample completed successfully!\n";
    return 0;
}