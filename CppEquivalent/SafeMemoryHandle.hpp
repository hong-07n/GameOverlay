#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

/// <summary>
/// C++ equivalent of SafeMemoryHandle - wrapper for Windows process memory access.
/// Provides safe methods to read various data types from process memory.
/// </summary>
class SafeMemoryHandle {
private:
    HANDLE processHandle;
    bool isValid;

public:
    /// <summary>
    /// Default constructor.
    /// </summary>
    SafeMemoryHandle();

    /// <summary>
    /// Constructor that opens a process by ID.
    /// </summary>
    /// <param name="processId">Process ID to open</param>
    explicit SafeMemoryHandle(DWORD processId);

    /// <summary>
    /// Destructor - automatically closes handle.
    /// </summary>
    ~SafeMemoryHandle();

    // Disable copy constructor and assignment operator
    SafeMemoryHandle(const SafeMemoryHandle&) = delete;
    SafeMemoryHandle& operator=(const SafeMemoryHandle&) = delete;

    // Enable move constructor and assignment operator
    SafeMemoryHandle(SafeMemoryHandle&& other) noexcept;
    SafeMemoryHandle& operator=(SafeMemoryHandle&& other) noexcept;

    /// <summary>
    /// Check if handle is valid.
    /// </summary>
    /// <returns>True if handle is valid, false otherwise</returns>
    bool IsValid() const { return isValid && processHandle != INVALID_HANDLE_VALUE; }

    /// <summary>
    /// Get the underlying Windows handle.
    /// </summary>
    /// <returns>Windows process handle</returns>
    HANDLE GetHandle() const { return processHandle; }

    /// <summary>
    /// Read process memory as type T.
    /// </summary>
    /// <typeparam name="T">Type of data structure to read</typeparam>
    /// <param name="address">Address to read from</param>
    /// <returns>Data from process memory in T format</returns>
    template<typename T>
    T ReadMemory(uintptr_t address) const;

    /// <summary>
    /// Read process memory as an array.
    /// </summary>
    /// <typeparam name="T">Array element type</typeparam>
    /// <param name="address">Memory address to read from</param>
    /// <param name="elementCount">Number of elements to read</param>
    /// <returns>Vector containing the read data</returns>
    template<typename T>
    std::vector<T> ReadMemoryArray(uintptr_t address, size_t elementCount) const;

    /// <summary>
    /// Read ASCII string from process memory.
    /// </summary>
    /// <param name="address">Address pointing to the string</param>
    /// <param name="maxLength">Maximum length to read (default 128)</param>
    /// <returns>String read from memory</returns>
    std::string ReadString(uintptr_t address, size_t maxLength = 128) const;

    /// <summary>
    /// Read Unicode (UTF-16) string from process memory.
    /// </summary>
    /// <param name="address">Address pointing to the Unicode string</param>
    /// <param name="maxLength">Maximum length to read in characters (default 128)</param>
    /// <returns>String read from memory (converted to UTF-8)</returns>
    std::string ReadUnicodeString(uintptr_t address, size_t maxLength = 128) const;

private:
    /// <summary>
    /// Initialize the handle.
    /// </summary>
    void Init();

    /// <summary>
    /// Close the handle safely.
    /// </summary>
    void Close();
};

// Template implementations

template<typename T>
T SafeMemoryHandle::ReadMemory(uintptr_t address) const {
    T result{};
    
    if (!IsValid() || address == 0) {
        return result;
    }

    SIZE_T bytesRead = 0;
    if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address), 
                          &result, sizeof(T), &bytesRead)) {
        DWORD error = GetLastError();
        std::cerr << "Failed to read memory at 0x" << std::hex << address 
                  << " - Error: 0x" << error << std::endl;
        return T{};
    }

    if (bytesRead != sizeof(T)) {
        std::cerr << "Partial read at 0x" << std::hex << address 
                  << " - Expected: " << sizeof(T) << ", Read: " << bytesRead << std::endl;
        return T{};
    }

    return result;
}

template<typename T>
std::vector<T> SafeMemoryHandle::ReadMemoryArray(uintptr_t address, size_t elementCount) const {
    if (!IsValid() || address == 0 || elementCount == 0) {
        return std::vector<T>();
    }

    std::vector<T> buffer(elementCount);
    SIZE_T totalBytes = elementCount * sizeof(T);
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address),
                          buffer.data(), totalBytes, &bytesRead)) {
        DWORD error = GetLastError();
        std::cerr << "Failed to read memory array at 0x" << std::hex << address 
                  << " - Error: 0x" << error << std::endl;
        return std::vector<T>();
    }

    if (bytesRead < totalBytes) {
        std::cerr << "Partial array read at 0x" << std::hex << address 
                  << " - Expected: " << totalBytes << ", Read: " << bytesRead << std::endl;
        // Resize buffer to actual elements read
        buffer.resize(bytesRead / sizeof(T));
    }

    return buffer;
}