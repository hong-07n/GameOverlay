#include "Pattern.h"
#include "StaticOffsetsPatterns.h"
#include <iostream>
#include <cassert>

void testPatternParsing() {
    std::cout << "Testing Pattern parsing..." << std::endl;
    
    // Test basic pattern without caret
    GameOffsets::Pattern pattern1("Test Pattern", "48 83 EC ?? 48 8B", 5);
    assert(pattern1.name == "Test Pattern");
    assert(pattern1.bytesToSkip == 5);
    assert(pattern1.data.size() == 6);
    assert(pattern1.mask.size() == 6);
    assert(pattern1.data[0] == 0x48);
    assert(pattern1.data[1] == 0x83);
    assert(pattern1.data[2] == 0xEC);
    assert(pattern1.data[3] == 0x00); // wildcard
    assert(pattern1.data[4] == 0x48);
    assert(pattern1.data[5] == 0x8B);
    assert(pattern1.mask[0] == true);
    assert(pattern1.mask[1] == true);
    assert(pattern1.mask[2] == true);
    assert(pattern1.mask[3] == false); // wildcard
    assert(pattern1.mask[4] == true);
    assert(pattern1.mask[5] == true);
    
    std::cout << "Pattern 1: " << pattern1.toString() << std::endl;
    
    // Test pattern with caret
    GameOffsets::Pattern pattern2("Test Pattern with Caret", "48 83 ^ EC ?? 48");
    assert(pattern2.name == "Test Pattern with Caret");
    assert(pattern2.bytesToSkip == 2); // Caret at position 2
    assert(pattern2.data.size() == 5); // Caret removed, but has 5 remaining bytes
    assert(pattern2.data[0] == 0x48);
    assert(pattern2.data[1] == 0x83);
    assert(pattern2.data[2] == 0xEC);
    assert(pattern2.data[3] == 0x00); // wildcard
    assert(pattern2.data[4] == 0x48);
    
    std::cout << "Pattern 2: " << pattern2.toString() << std::endl;
    
    std::cout << "Pattern parsing tests passed!" << std::endl;
}

void testHexParsing() {
    std::cout << "Testing hex parsing..." << std::endl;
    
    // Test various hex formats
    GameOffsets::Pattern pattern1("Hex Test", "0x48 0x83 EC, F1");
    assert(pattern1.data[0] == 0x48);
    assert(pattern1.data[1] == 0x83);
    assert(pattern1.data[2] == 0xEC);
    assert(pattern1.data[3] == 0xF1);
    
    std::cout << "Hex parsing tests passed!" << std::endl;
}

void testStaticPatterns() {
    std::cout << "Testing static patterns..." << std::endl;
    
    const auto& patterns = GameOffsets::StaticOffsetsPatterns::Patterns;
    assert(patterns.size() == 6);
    
    // Test first pattern
    const auto& gameStatesPattern = patterns[0];
    assert(gameStatesPattern.name == "Game States");
    assert(gameStatesPattern.bytesToSkip == 12); // Caret position in the pattern
    
    std::cout << "Game States Pattern: " << gameStatesPattern.toString() << std::endl;
    
    // Test all patterns have valid names
    for (const auto& pattern : patterns) {
        assert(!pattern.name.empty());
        assert(!pattern.data.empty());
        assert(pattern.data.size() == pattern.mask.size());
        std::cout << "Pattern: " << pattern.name << " - Size: " << pattern.data.size() 
                  << " - BytesToSkip: " << pattern.bytesToSkip << std::endl;
    }
    
    std::cout << "Static patterns tests passed!" << std::endl;
}

int main() {
    try {
        std::cout << "Starting GameOffsets Pattern Tests" << std::endl;
        std::cout << "====================================" << std::endl;
        
        testPatternParsing();
        std::cout << std::endl;
        
        testHexParsing();
        std::cout << std::endl;
        
        testStaticPatterns();
        std::cout << std::endl;
        
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}