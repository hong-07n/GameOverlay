#include "SafeMemoryHandle.hpp"
#include <tlhelp32.h>
#include <codecvt>
#include <locale>

SafeMemoryHandle::SafeMemoryHandle() : processHandle(INVALID_HANDLE_VALUE), isValid(false) {
    std::cout << "Opening a new handle." << std::endl;
}

SafeMemoryHandle::SafeMemoryHandle(DWORD processId) : processHandle(INVALID_HANDLE_VALUE), isValid(false) {
    processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processId);
    
    if (processHandle == nullptr || processHandle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::cout << "Failed to open a new handle for process " << processId 
                  << " due to Error: 0x" << std::hex << error << std::endl;
        isValid = false;
    } else {
        std::cout << "Opened a new handle using HANDLE 0x" << std::hex 
                  << reinterpret_cast<uintptr_t>(processHandle) << std::endl;
        isValid = true;
    }
}

SafeMemoryHandle::~SafeMemoryHandle() {
    Close();
}

SafeMemoryHandle::SafeMemoryHandle(SafeMemoryHandle&& other) noexcept 
    : processHandle(other.processHandle), isValid(other.isValid) {
    other.processHandle = INVALID_HANDLE_VALUE;
    other.isValid = false;
}

SafeMemoryHandle& SafeMemoryHandle::operator=(SafeMemoryHandle&& other) noexcept {
    if (this != &other) {
        Close();
        processHandle = other.processHandle;
        isValid = other.isValid;
        other.processHandle = INVALID_HANDLE_VALUE;
        other.isValid = false;
    }
    return *this;
}

std::string SafeMemoryHandle::ReadString(uintptr_t address, size_t maxLength) const {
    auto buffer = ReadMemoryArray<char>(address, maxLength);
    if (buffer.empty()) {
        return std::string();
    }

    // Find null terminator
    auto nullPos = std::find(buffer.begin(), buffer.end(), '\0');
    if (nullPos != buffer.end()) {
        return std::string(buffer.begin(), nullPos);
    }

    return std::string(buffer.begin(), buffer.end());
}

std::string SafeMemoryHandle::ReadUnicodeString(uintptr_t address, size_t maxLength) const {
    auto buffer = ReadMemoryArray<wchar_t>(address, maxLength);
    if (buffer.empty()) {
        return std::string();
    }

    // Find null terminator
    auto nullPos = std::find(buffer.begin(), buffer.end(), L'\0');
    if (nullPos != buffer.end()) {
        std::wstring wstr(buffer.begin(), nullPos);
        
        // Convert to UTF-8
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.to_bytes(wstr);
        } catch (const std::exception& e) {
            std::cerr << "Unicode conversion error: " << e.what() << std::endl;
            return std::string();
        }
    }

    try {
        std::wstring wstr(buffer.begin(), buffer.end());
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    } catch (const std::exception& e) {
        std::cerr << "Unicode conversion error: " << e.what() << std::endl;
        return std::string();
    }
}

void SafeMemoryHandle::Init() {
    processHandle = INVALID_HANDLE_VALUE;
    isValid = false;
}

void SafeMemoryHandle::Close() {
    if (processHandle != INVALID_HANDLE_VALUE && processHandle != nullptr) {
        std::cout << "Releasing handle on 0x" << std::hex 
                  << reinterpret_cast<uintptr_t>(processHandle) << std::endl;
        CloseHandle(processHandle);
        processHandle = INVALID_HANDLE_VALUE;
    }
    isValid = false;
}