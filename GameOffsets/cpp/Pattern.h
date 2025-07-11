#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

namespace GameOffsets {

    /**
     * @brief A struct representing a pattern used for memory scanning
     */
    struct Pattern {
        /// User friendly name for the pattern
        std::string name;
        
        /// Pattern bytes
        std::vector<uint8_t> data;
        
        /// Return true if the byte needs to be checked, otherwise
        /// returns false (for a wildcard entry)
        std::vector<bool> mask;
        
        /// Number of bytes to skip in the offset returned from the pattern
        /// in order to reach the static-address offset data
        int bytesToSkip;

        /**
         * @brief Default constructor
         */
        Pattern();

        /**
         * @brief Create a new instance of the Pattern with auto-calculated BytesToSkip
         * @param name User friendly name for the pattern
         * @param arrayOfHexBytes Array of HEX Bytes with "^" in it to calculate the bytes to skip
         */
        Pattern(const std::string& name, const std::string& arrayOfHexBytes);

        /**
         * @brief Create a new instance of the Pattern
         * @param name User friendly name for the pattern
         * @param arrayOfHexBytes Array of HEX Bytes
         * @param bytesToSkip Number of bytes to skip to reach the static-address offset data
         */
        Pattern(const std::string& name, const std::string& arrayOfHexBytes, int bytesToSkip);

        /**
         * @brief Pretty prints the Pattern
         * @return Pattern in string format
         */
        std::string toString() const;

    private:
        /**
         * @brief Parses the Array of bytes in HEX format and converts it into
         * a byte array and a mask (in bool format) array
         * @param arrayOfHexBytes Vector of hex byte strings
         * @return Pair containing byte array and mask array
         */
        static std::pair<std::vector<uint8_t>, std::vector<bool>> parseArrayOfHexBytes(
            const std::vector<std::string>& arrayOfHexBytes);

        /**
         * @brief Splits a string by given delimiters
         * @param str String to split
         * @param delimiters Characters to split by
         * @return Vector of split strings
         */
        static std::vector<std::string> split(const std::string& str, const std::string& delimiters);

        /**
         * @brief Converts a hex string to byte value
         * @param hexStr Hex string to convert
         * @return Byte value
         */
        static uint8_t hexStringToByte(const std::string& hexStr);
    };

} // namespace GameOffsets