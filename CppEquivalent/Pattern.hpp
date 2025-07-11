#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

/// <summary>
/// C++ equivalent of the C# Pattern struct.
/// Contains pattern data, mask, and metadata for pattern scanning.
/// </summary>
struct Pattern {
    /// <summary>
    /// User friendly name for the pattern.
    /// </summary>
    std::string name;

    /// <summary>
    /// Pattern bytes array.
    /// </summary>
    std::vector<uint8_t> data;

    /// <summary>
    /// Mask array - true if byte needs to be checked, false for wildcards.
    /// </summary>
    std::vector<bool> mask;

    /// <summary>
    /// Number of bytes to skip in the offset returned from the pattern
    /// in order to reach the static-address offset data.
    /// </summary>
    int bytesToSkip;

    /// <summary>
    /// Create a new Pattern instance.
    /// </summary>
    /// <param name="name">User friendly name for the pattern</param>
    /// <param name="hexString">Hex bytes separated by space/comma, use ?? or ? for wildcards, ^ for bytesToSkip calculation</param>
    Pattern(const std::string& name, const std::string& hexString);

    /// <summary>
    /// Create a new Pattern instance with explicit bytesToSkip.
    /// </summary>
    /// <param name="name">User friendly name for the pattern</param>
    /// <param name="hexString">Hex bytes separated by space/comma, use ?? or ? for wildcards</param>
    /// <param name="bytesToSkip">Number of bytes to skip</param>
    Pattern(const std::string& name, const std::string& hexString, int bytesToSkip);

    /// <summary>
    /// Get string representation of the pattern.
    /// </summary>
    std::string toString() const;

private:
    /// <summary>
    /// Parse hex string into data and mask arrays.
    /// </summary>
    /// <param name="hexTokens">Vector of hex tokens</param>
    /// <returns>Pair of data vector and mask vector</returns>
    static std::pair<std::vector<uint8_t>, std::vector<bool>> parseHexBytes(const std::vector<std::string>& hexTokens);

    /// <summary>
    /// Split string by delimiters.
    /// </summary>
    /// <param name="str">String to split</param>
    /// <param name="delimiters">Delimiter characters</param>
    /// <returns>Vector of tokens</returns>
    static std::vector<std::string> split(const std::string& str, const std::string& delimiters);

    /// <summary>
    /// Trim whitespace from string.
    /// </summary>
    /// <param name="str">String to trim</param>
    /// <returns>Trimmed string</returns>
    static std::string trim(const std::string& str);
};