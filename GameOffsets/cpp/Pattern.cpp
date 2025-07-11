#include "Pattern.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <iomanip>

namespace GameOffsets {

    Pattern::Pattern() : bytesToSkip(0) {}

    Pattern::Pattern(const std::string& name, const std::string& arrayOfHexBytes) 
        : name(name) {
        auto tokens = split(arrayOfHexBytes, " ,");
        
        // Find the '^' marker to calculate bytesToSkip
        auto caretPos = std::find(tokens.begin(), tokens.end(), "^");
        if (caretPos != tokens.end()) {
            bytesToSkip = static_cast<int>(std::distance(tokens.begin(), caretPos));
            tokens.erase(caretPos); // Remove the '^' marker
        } else {
            bytesToSkip = 0;
        }
        
        auto parsed = parseArrayOfHexBytes(tokens);
        data = std::move(parsed.first);
        mask = std::move(parsed.second);
    }

    Pattern::Pattern(const std::string& name, const std::string& arrayOfHexBytes, int bytesToSkip)
        : name(name), bytesToSkip(bytesToSkip) {
        auto tokens = split(arrayOfHexBytes, " ,");
        auto parsed = parseArrayOfHexBytes(tokens);
        data = std::move(parsed.first);
        mask = std::move(parsed.second);
    }

    std::string Pattern::toString() const {
        std::ostringstream oss;
        oss << "Name: " << name << " Pattern: ";
        
        for (size_t i = 0; i < data.size(); ++i) {
            if (mask[i]) {
                oss << "0x" << std::hex << std::uppercase << static_cast<int>(data[i]) << " ";
            } else {
                oss << "?? ";
            }
        }
        
        oss << "BytesToSkip: " << std::dec << bytesToSkip;
        return oss.str();
    }

    std::pair<std::vector<uint8_t>, std::vector<bool>> Pattern::parseArrayOfHexBytes(
        const std::vector<std::string>& arrayOfHexBytes) {
        
        std::vector<bool> maskResult;
        std::vector<uint8_t> dataResult;
        
        for (const auto& hexByte : arrayOfHexBytes) {
            if (hexByte.empty()) continue;
            
            if (hexByte.front() == '?') {
                dataResult.push_back(0x00);
                maskResult.push_back(false);
            } else {
                dataResult.push_back(hexStringToByte(hexByte));
                maskResult.push_back(true);
            }
        }
        
        return std::make_pair(std::move(dataResult), std::move(maskResult));
    }

    std::vector<std::string> Pattern::split(const std::string& str, const std::string& delimiters) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = 0;
        
        while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
            if (end != start) {
                tokens.push_back(str.substr(start, end - start));
            }
            start = end + 1;
        }
        
        if (start < str.length()) {
            tokens.push_back(str.substr(start));
        }
        
        return tokens;
    }

    uint8_t Pattern::hexStringToByte(const std::string& hexStr) {
        std::string cleanHex = hexStr;
        
        // Remove "0x" or "0X" prefix if present
        if (cleanHex.length() >= 2 && cleanHex.substr(0, 2) == "0x") {
            cleanHex = cleanHex.substr(2);
        } else if (cleanHex.length() >= 2 && cleanHex.substr(0, 2) == "0X") {
            cleanHex = cleanHex.substr(2);
        }
        
        // Convert to uppercase for consistent parsing
        std::transform(cleanHex.begin(), cleanHex.end(), cleanHex.begin(), ::toupper);
        
        try {
            unsigned long value = std::stoul(cleanHex, nullptr, 16);
            if (value > 0xFF) {
                throw std::out_of_range("Hex value too large for byte");
            }
            return static_cast<uint8_t>(value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid hex string: " + hexStr);
        }
    }

} // namespace GameOffsets