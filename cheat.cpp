#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (_wcsicmp(modEntry.szModule, modName) == 0) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "    PvZ 1.2 GOTY Cheat v1.0    \n";
    std::cout << "==================================================\n\n";

    HWND hGameWindow = FindWindowA(NULL, "Plants vs. Zombies");
    if (!hGameWindow) {
        std::cout << "[!] Game not found! Please open Plants vs. Zombies first.\n";
        system("pause");
        return 0;
    }

    DWORD procId = 0;
    GetWindowThreadProcessId(hGameWindow, &procId);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (!hProcess) {
        std::cout << "[!] Failed to open handle. Run as Administrator!\n";
        system("pause");
        return 0;
    }

    uintptr_t baseAddress = GetModuleBaseAddress(procId, L"popcapgame1.exe");
    if (!baseAddress) {
        std::cout << "[!] Failed to locate popcapgame1.exe module base.\n";
        CloseHandle(hProcess);
        return 0;
    }

    uintptr_t sunAssemblyAddr = baseAddress + 0x1E6E0;      
    uintptr_t cooldownJumpAddr = baseAddress + 0x91E55;     

    std::vector<BYTE> cooldownCheatBytes = { 0x90, 0x90 };
    std::vector<BYTE> cooldownOriginalBytes = { 0x7E, 0x14 };

    LPVOID newmem = VirtualAllocEx(hProcess, nullptr, 2048, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!newmem) {
        std::cout << "[!] Failed to allocate script memory block.\n";
        CloseHandle(hProcess);
        return 0;
    }

    uintptr_t jmpToNewmemOffset = ((uintptr_t)newmem - sunAssemblyAddr) - 5;
    uintptr_t jmpBackOffset = (sunAssemblyAddr + 6) - ((uintptr_t)newmem + 10) - 5;

    BYTE payloadBytes[15] = {
        0xC7, 0x80, 0x78, 0x55, 0x00, 0x00, 0x0F, 0x27, 0x00, 0x00, 
        0xE9, 0x00, 0x00, 0x00, 0x00                                
    };
    memcpy(&payloadBytes[11], &jmpBackOffset, sizeof(jmpBackOffset));

    BYTE hookBytes[6] = { 0xE9, 0x00, 0x00, 0x00, 0x00, 0x90 };
    memcpy(&hookBytes[1], &jmpToNewmemOffset, sizeof(jmpToNewmemOffset)); 

    BYTE sunOriginalBytes[6] = { 0x01, 0x88, 0x78, 0x55, 0x00, 0x00 };

    WriteProcessMemory(hProcess, newmem, payloadBytes, sizeof(payloadBytes), nullptr);

    std::cout << "[+] Cheat loaded successfully!\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "[F1] Toggle Infinite Sun\n";
    std::cout << "[F2] Toggle Global Zero Cooldown\n";
    std::cout << "[ESC] Exit \n";
    std::cout << "--------------------------------------------------\n\n";

    bool sunStatus = false;
    bool cooldownStatus = false;

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 1) break;

        if (GetAsyncKeyState(VK_F1) & 1) {
            sunStatus = !sunStatus;
            if (sunStatus) {
                WriteProcessMemory(hProcess, (LPVOID)sunAssemblyAddr, hookBytes, sizeof(hookBytes), nullptr);
                std::cout << "[>] Infinite Sun: ENABLED\n";
            }
            else {
                WriteProcessMemory(hProcess, (LPVOID)sunAssemblyAddr, sunOriginalBytes, sizeof(sunOriginalBytes), nullptr);
                std::cout << "[>] Infinite Sun: DISABLED\n";
            }
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            cooldownStatus = !cooldownStatus;
            if (cooldownStatus) {
                WriteProcessMemory(hProcess, (LPVOID)cooldownJumpAddr, cooldownCheatBytes.data(), cooldownCheatBytes.size(), nullptr);
                std::cout << "[>] Global Instant Cooldown: ENABLED\n";
            }
            else {
                WriteProcessMemory(hProcess, (LPVOID)cooldownJumpAddr, cooldownOriginalBytes.data(), cooldownOriginalBytes.size(), nullptr);
                std::cout << "[>] Global Instant Cooldown: DISABLED\n";
            }
        }

        Sleep(10);
    }

    WriteProcessMemory(hProcess, (LPVOID)sunAssemblyAddr, sunOriginalBytes, sizeof(sunOriginalBytes), nullptr);
    WriteProcessMemory(hProcess, (LPVOID)cooldownJumpAddr, cooldownOriginalBytes.data(), cooldownOriginalBytes.size(), nullptr);
    VirtualFreeEx(hProcess, newmem, 0, MEM_RELEASE); 

    CloseHandle(hProcess);
    return 0;
}
