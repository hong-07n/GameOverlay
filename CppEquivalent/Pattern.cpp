#include "Pattern.hpp"
#include <iomanip>

Pattern::Pattern(const std::string& name, const std::string& hexString) : name(name) {
    auto tokens = split(hexString, " ,");
    
    // Find bytesToSkip from ^ marker
    auto it = std::find(tokens.begin(), tokens.end(), "^");
    if (it != tokens.end()) {
        bytesToSkip = static_cast<int>(std::distance(tokens.begin(), it));
        tokens.erase(it); // Remove ^ from tokens
    } else {
        bytesToSkip = 0;
    }
    
    auto parsed = parseHexBytes(tokens);
    data = std::move(parsed.first);
    mask = std::move(parsed.second);
}

Pattern::Pattern(const std::string& name, const std::string& hexString, int bytesToSkip) 
    : name(name), bytesToSkip(bytesToSkip) {
    auto tokens = split(hexString, " ,");
    auto parsed = parseHexBytes(tokens);
    data = std::move(parsed.first);
    mask = std::move(parsed.second);
}

std::string Pattern::toString() const {
    std::ostringstream oss;
    oss << "Name: " << name << " Pattern: ";
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (mask[i]) {
            oss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                << static_cast<unsigned>(data[i]) << " ";
        } else {
            oss << "?? ";
        }
    }
    
    oss << "BytesToSkip: " << std::dec << bytesToSkip;
    return oss.str();
}

std::pair<std::vector<uint8_t>, std::vector<bool>> Pattern::parseHexBytes(const std::vector<std::string>& hexTokens) {
    std::vector<uint8_t> data;
    std::vector<bool> mask;
    
    for (const auto& token : hexTokens) {
        if (token.empty()) continue;
        
        if (token[0] == '?') {
            data.push_back(0x00);
            mask.push_back(false);
        } else {
            try {
                // Remove 0x prefix if present
                std::string hexStr = token;
                if (hexStr.size() >= 2 && hexStr.substr(0, 2) == "0x") {
                    hexStr = hexStr.substr(2);
                }
                
                uint8_t byte = static_cast<uint8_t>(std::stoul(hexStr, nullptr, 16));
                data.push_back(byte);
                mask.push_back(true);
            } catch (const std::exception&) {
                // Invalid hex, treat as wildcard
                data.push_back(0x00);
                mask.push_back(false);
            }
        }
    }
    
    return std::make_pair(std::move(data), std::move(mask));
}

std::vector<std::string> Pattern::split(const std::string& str, const std::string& delimiters) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    
    while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
        if (end != start) {
            tokens.push_back(trim(str.substr(start, end - start)));
        }
        start = end + 1;
    }
    
    if (start < str.length()) {
        tokens.push_back(trim(str.substr(start)));
    }
    
    return tokens;
}

std::string Pattern::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}