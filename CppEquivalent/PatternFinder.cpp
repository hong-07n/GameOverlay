#include "PatternFinder.hpp"
#include <iostream>
#include <stdexcept>
#include <execution>
#include <numeric>

std::unordered_map<std::string, uintptr_t> PatternFinder::Find(
    const SafeMemoryHandle& handle,
    uintptr_t baseAddress,
    size_t processSize,
    const std::vector<Pattern>& patterns
) {
    if (patterns.empty()) {
        return std::unordered_map<std::string, uintptr_t>();
    }

    // Get the maximum pattern length for overlap handling
    size_t patternMaxLength = GetBiggestPatternLength(patterns);
    
    // Calculate total read operations needed
    size_t totalReadOperations = CalculateTotalReadOperations(processSize);
    
    std::unordered_map<std::string, uintptr_t> result;
    
    size_t totalPatterns = patterns.size();
    std::vector<std::atomic<bool>> isPatternFound(totalPatterns);
    std::vector<std::atomic<uintptr_t>> patternOffsets(totalPatterns);
    std::atomic<size_t> totalPatternsFound{0};
    
    // Initialize atomic arrays
    for (size_t i = 0; i < totalPatterns; ++i) {
        isPatternFound[i].store(false);
        patternOffsets[i].store(0);
    }

    // Create vector for parallel processing
    std::vector<size_t> readIndices(totalReadOperations);
    std::iota(readIndices.begin(), readIndices.end(), 0);

    // Use parallel for_each to process memory chunks
    std::for_each(std::execution::par_unseq, readIndices.begin(), readIndices.end(),
        [&](size_t i) {
            // Early exit if all patterns found
            if (totalPatternsFound.load() >= totalPatterns) {
                return;
            }

            size_t currentOffset = i * MAX_BYTES_OBJECT;
            bool isLastIteration = (i == totalReadOperations - 1);
            size_t actualReadSize = isLastIteration ? 
                processSize - currentOffset : MAX_BYTES_OBJECT;

            // Add pattern overlap handling for non-last iterations
            if (!isLastIteration) {
                actualReadSize += patternMaxLength;
                // Ensure we don't read beyond process bounds
                if (currentOffset + actualReadSize > processSize) {
                    actualReadSize = processSize - currentOffset;
                }
            }

            // Read memory chunk
            auto processData = handle.ReadMemoryArray<uint8_t>(
                baseAddress + currentOffset, actualReadSize);
            
            if (processData.empty()) {
                return;
            }

            // Search for patterns in this chunk
            SearchPatternsInChunk(
                processData.data(),
                processData.size(),
                patterns,
                currentOffset,
                isPatternFound,
                patternOffsets,
                totalPatternsFound
            );
        }
    );

    // Check if all patterns were found
    if (totalPatternsFound.load() < totalPatterns) {
        throw std::runtime_error("Couldn't find some patterns. Please fix the patterns.");
    }

    // Verify uniqueness
    for (size_t i = 0; i < totalPatterns; ++i) {
        if (!isPatternFound[i].load()) {
            throw std::runtime_error("There is a non-unique pattern. Please fix the patterns.");
        }
    }

    // Build result map
    for (size_t i = 0; i < totalPatterns; ++i) {
        result[patterns[i].name] = patternOffsets[i].load() + patterns[i].bytesToSkip;
    }

    return result;
}

size_t PatternFinder::GetBiggestPatternLength(const std::vector<Pattern>& patterns) {
    size_t maxLength = 0;
    for (const auto& pattern : patterns) {
        if (pattern.data.size() > maxLength) {
            maxLength = pattern.data.size();
        }
    }
    return maxLength;
}

size_t PatternFinder::CalculateTotalReadOperations(size_t processSize) {
    size_t operations = processSize / MAX_BYTES_OBJECT;
    return (processSize % MAX_BYTES_OBJECT == 0) ? operations : operations + 1;
}

bool PatternFinder::SearchPatternsInChunk(
    const uint8_t* data,
    size_t dataSize,
    const std::vector<Pattern>& patterns,
    size_t currentOffset,
    std::vector<std::atomic<bool>>& foundPatterns,
    std::vector<std::atomic<uintptr_t>>& patternOffsets,
    std::atomic<size_t>& totalPatternsFound
) {
    // Create vector of positions for parallel search
    std::vector<size_t> positions(dataSize);
    std::iota(positions.begin(), positions.end(), 0);

    std::for_each(std::execution::par_unseq, positions.begin(), positions.end(),
        [&](size_t j) {
            // Early exit if all patterns found
            if (totalPatternsFound.load() >= patterns.size()) {
                return;
            }

            // Check each pattern at this position
            for (size_t k = 0; k < patterns.size(); ++k) {
                if (foundPatterns[k].load()) {
                    continue;
                }

                const auto& pattern = patterns[k];
                
                if (MatchPattern(data, dataSize, j, pattern)) {
                    // Pattern found! Mark it as found
                    bool expected = false;
                    if (foundPatterns[k].compare_exchange_strong(expected, true)) {
                        // First thread to find this pattern
                        patternOffsets[k].store(currentOffset + j);
                        totalPatternsFound.fetch_add(1);
                    }
                }
            }
        }
    );

    return totalPatternsFound.load() >= patterns.size();
}

bool PatternFinder::MatchPattern(
    const uint8_t* data,
    size_t dataSize,
    size_t position,
    const Pattern& pattern
) {
    size_t patternLength = pattern.data.size();
    
    // Check if we have enough data left
    if (dataSize - position < patternLength) {
        return false;
    }

    // Optimized pattern matching similar to C# version
    bool isOddLengthPattern = (patternLength % 2) != 0;
    if (isOddLengthPattern) {
        size_t middleOfOddLengthPattern = patternLength / 2;
        if (!pattern.mask[middleOfOddLengthPattern] && 
            data[position + middleOfOddLengthPattern] == pattern.data[middleOfOddLengthPattern]) {
            return false;
        }
    }

    // Check from both ends towards middle for better cache locality
    for (size_t l = 0; l < patternLength / 2; ++l) {
        // Check from start
        if (pattern.mask[l] && data[position + l] != pattern.data[l]) {
            return false;
        }

        // Check from end
        size_t last = patternLength - (l + 1);
        if (pattern.mask[last] && data[position + last] != pattern.data[last]) {
            return false;
        }
    }

    return true;
}