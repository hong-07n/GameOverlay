#include "PatternFinder.hpp"
#include "SafeMemoryHandle.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

/// <summary>
/// Helper function to find process ID by name.
/// </summary>
/// <param name="processName">Name of the process (e.g., "notepad.exe")</param>
/// <returns>Process ID or 0 if not found</returns>
DWORD FindProcessId(const std::wstring& processName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return 0;
    }

    do {
        if (processName == pe32.szExeFile) {
            CloseHandle(hProcessSnap);
            return pe32.th32ProcessID;
        }
    } while (Process32NextW(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return 0;
}

/// <summary>
/// Get base address and size of main module for a process.
/// </summary>
/// <param name="processId">Process ID</param>
/// <param name="baseAddress">Output: base address of main module</param>
/// <param name="moduleSize">Output: size of main module</param>
/// <returns>True if successful, false otherwise</returns>
bool GetProcessModuleInfo(DWORD processId, uintptr_t& baseAddress, size_t& moduleSize) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return false;
    }

    HMODULE hModules[1024];
    DWORD cbNeeded;
    
    if (!EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        CloseHandle(hProcess);
        return false;
    }

    // Get main module (first one)
    MODULEINFO moduleInfo;
    if (!GetModuleInformation(hProcess, hModules[0], &moduleInfo, sizeof(moduleInfo))) {
        CloseHandle(hProcess);
        return false;
    }

    baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
    moduleSize = moduleInfo.SizeOfImage;

    CloseHandle(hProcess);
    return true;
}

/// <summary>
/// Create sample patterns similar to those in StaticOffsetsPatterns.cs
/// </summary>
std::vector<Pattern> CreateSamplePatterns() {
    std::vector<Pattern> patterns;
    
    // Sample patterns - in real usage, these would be actual game patterns
    patterns.emplace_back("Sample Pattern 1", "48 83 EC ?? 48 8B F1 33 ED 48 39 2D ^ ?? ?? ?? ??");
    patterns.emplace_back("Sample Pattern 2", "48 ?? ?? ^ ?? ?? ?? ?? 41 ?? ?? ?? 39 ?? ?? ?? ?? ?? 0F 8E");
    patterns.emplace_back("Sample Pattern 3", "FF ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 05 ^ ?? ?? ?? ?? ?? 8D ?? ?? ?? 8B ??");
    
    return patterns;
}

int main() {
    std::cout << "=== C++ PatternFinder and SafeMemoryHandle Usage Example ===" << std::endl;
    
    try {
        // Example 1: Basic SafeMemoryHandle usage
        std::cout << "\n1. Basic SafeMemoryHandle usage:" << std::endl;
        
        // Find notepad process (you can change this to any process you want to test with)
        std::wstring targetProcess = L"notepad.exe";
        DWORD processId = FindProcessId(targetProcess);
        
        if (processId == 0) {
            std::cout << "Process '" << std::string(targetProcess.begin(), targetProcess.end()) 
                      << "' not found. Please start notepad.exe to test memory reading." << std::endl;
        } else {
            std::cout << "Found process ID: " << processId << std::endl;
            
            // Create memory handle
            SafeMemoryHandle memHandle(processId);
            
            if (memHandle.IsValid()) {
                std::cout << "Successfully opened process handle." << std::endl;
                
                // Get process module info
                uintptr_t baseAddress;
                size_t moduleSize;
                
                if (GetProcessModuleInfo(processId, baseAddress, moduleSize)) {
                    std::cout << "Base address: 0x" << std::hex << baseAddress << std::endl;
                    std::cout << "Module size: " << std::dec << moduleSize << " bytes" << std::endl;
                    
                    // Example: Read first few bytes of the process
                    auto headerBytes = memHandle.ReadMemoryArray<uint8_t>(baseAddress, 16);
                    if (!headerBytes.empty()) {
                        std::cout << "First 16 bytes: ";
                        for (auto byte : headerBytes) {
                            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                      << static_cast<unsigned>(byte) << " ";
                        }
                        std::cout << std::endl;
                    }
                } else {
                    std::cout << "Failed to get process module information." << std::endl;
                }
            } else {
                std::cout << "Failed to open process handle. Try running as administrator." << std::endl;
            }
        }
        
        // Example 2: Pattern creation and usage
        std::cout << "\n2. Pattern creation and usage:" << std::endl;
        
        auto patterns = CreateSamplePatterns();
        
        std::cout << "Created " << patterns.size() << " sample patterns:" << std::endl;
        for (const auto& pattern : patterns) {
            std::cout << "  " << pattern.toString() << std::endl;
        }
        
        // Example 3: PatternFinder usage (with mock data since we need a real process)
        std::cout << "\n3. PatternFinder usage example:" << std::endl;
        
        if (processId != 0) {
            SafeMemoryHandle memHandle(processId);
            
            if (memHandle.IsValid()) {
                uintptr_t baseAddress;
                size_t moduleSize;
                
                if (GetProcessModuleInfo(processId, baseAddress, moduleSize)) {
                    try {
                        // Note: In real usage, you would use actual game patterns
                        // For this demo, we'll catch the exception since our sample patterns
                        // likely won't be found in notepad.exe
                        auto results = PatternFinder::Find(memHandle, baseAddress, moduleSize, patterns);
                        
                        std::cout << "Found " << results.size() << " patterns:" << std::endl;
                        for (const auto& [name, offset] : results) {
                            std::cout << "  " << name << ": 0x" << std::hex << offset << std::endl;
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Pattern search failed (expected for demo): " << e.what() << std::endl;
                        std::cout << "In real usage, use actual game patterns that exist in the target process." << std::endl;
                    }
                }
            }
        }
        
        // Example 4: Custom pattern matching
        std::cout << "\n4. Custom pattern example:" << std::endl;
        
        // Create a simple pattern with wildcards
        Pattern customPattern("Custom Pattern", "4D 5A ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 50 45", 16);
        std::cout << "Custom pattern: " << customPattern.toString() << std::endl;
        
        std::cout << "\n=== Usage Summary ===" << std::endl;
        std::cout << "1. Create SafeMemoryHandle with process ID" << std::endl;
        std::cout << "2. Create Pattern objects with hex strings and wildcards (??)" << std::endl;
        std::cout << "3. Use PatternFinder::Find() to search for patterns in process memory" << std::endl;
        std::cout << "4. Access results as name->offset mapping" << std::endl;
        std::cout << "5. Use SafeMemoryHandle methods to read various data types from found offsets" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nExample completed successfully!" << std::endl;
    return 0;
}