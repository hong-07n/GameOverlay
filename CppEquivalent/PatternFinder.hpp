#pragma once
#include "Pattern.hpp"
#include "SafeMemoryHandle.hpp"
#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <algorithm>

/// <summary>
/// C++ equivalent of PatternFinder class.
/// Contains helper functions to find patterns (array of bytes in HEX) in process memory.
/// Uses parallel processing for improved performance and memory efficiency.
/// </summary>
class PatternFinder {
private:
    /// <summary>
    /// Maximum bytes to read at once to avoid large object heap issues.
    /// Corresponds to C# MaxBytesObject constant.
    /// </summary>
    static constexpr size_t MAX_BYTES_OBJECT = 84000;

public:
    /// <summary>
    /// Find all patterns in the process memory.
    /// </summary>
    /// <param name="handle">Handle to the process</param>
    /// <param name="baseAddress">Base address of the process main module</param>
    /// <param name="processSize">Total size of the process main module</param>
    /// <param name="patterns">Vector of patterns to search for</param>
    /// <returns>Map of pattern names to their found offsets</returns>
    static std::unordered_map<std::string, uintptr_t> Find(
        const SafeMemoryHandle& handle,
        uintptr_t baseAddress,
        size_t processSize,
        const std::vector<Pattern>& patterns
    );

private:
    /// <summary>
    /// Get the length of the biggest pattern.
    /// </summary>
    /// <param name="patterns">Vector of patterns</param>
    /// <returns>Length of the biggest pattern</returns>
    static size_t GetBiggestPatternLength(const std::vector<Pattern>& patterns);

    /// <summary>
    /// Calculate total number of read operations required.
    /// </summary>
    /// <param name="processSize">Size of the process main module</param>
    /// <returns>Total number of read operations required</returns>
    static size_t CalculateTotalReadOperations(size_t processSize);

    /// <summary>
    /// Search for patterns in a memory chunk.
    /// </summary>
    /// <param name="data">Memory data to search in</param>
    /// <param name="dataSize">Size of the memory data</param>
    /// <param name="patterns">Patterns to search for</param>
    /// <param name="currentOffset">Current offset in the process memory</param>
    /// <param name="foundPatterns">Atomic flags indicating which patterns were found</param>
    /// <param name="patternOffsets">Array to store found pattern offsets</param>
    /// <param name="totalPatternsFound">Atomic counter for total patterns found</param>
    /// <returns>True if all patterns were found, false otherwise</returns>
    static bool SearchPatternsInChunk(
        const uint8_t* data,
        size_t dataSize,
        const std::vector<Pattern>& patterns,
        size_t currentOffset,
        std::vector<std::atomic<bool>>& foundPatterns,
        std::vector<std::atomic<uintptr_t>>& patternOffsets,
        std::atomic<size_t>& totalPatternsFound
    );

    /// <summary>
    /// Check if a pattern matches at a specific position.
    /// </summary>
    /// <param name="data">Memory data</param>
    /// <param name="dataSize">Size of memory data</param>
    /// <param name="position">Position to check</param>
    /// <param name="pattern">Pattern to match</param>
    /// <returns>True if pattern matches, false otherwise</returns>
    static bool MatchPattern(
        const uint8_t* data,
        size_t dataSize,
        size_t position,
        const Pattern& pattern
    );
};