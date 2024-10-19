#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>

void SetConsoleColor(int textColor, int backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((backgroundColor << 4) | textColor));
}

void UpdateDeliveryOptimizationKeys() {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_USERS, TEXT("S-1-5-20\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\DeliveryOptimization\\Settings"), 0, KEY_SET_VALUE, &hKey);

    if (result == ERROR_SUCCESS) {
        DWORD downloadMode = 0;
        RegSetValueEx(hKey, TEXT("DownloadMode"), 0, REG_DWORD, (const BYTE*)&downloadMode, sizeof(downloadMode));

        DWORD downloadModeProvider = 8;
        RegSetValueEx(hKey, TEXT("DownloadModeProvider"), 0, REG_DWORD, (const BYTE*)&downloadModeProvider, sizeof(downloadModeProvider));

        DWORD downloadRateBackgroundPct = 100;
        RegSetValueEx(hKey, TEXT("DownloadRateBackgroundPct"), 0, REG_DWORD, (const BYTE*)&downloadRateBackgroundPct, sizeof(downloadRateBackgroundPct));

        DWORD downloadRateBackgroundProvider = 8;
        RegSetValueEx(hKey, TEXT("DownloadRateBackgroundProvider"), 0, REG_DWORD, (const BYTE*)&downloadRateBackgroundProvider, sizeof(downloadRateBackgroundProvider));

        DWORD downloadRateForegroundPct = 100;
        RegSetValueEx(hKey, TEXT("DownloadRateForegroundPct"), 0, REG_DWORD, (const BYTE*)&downloadRateForegroundPct, sizeof(downloadRateForegroundPct));

        DWORD downloadRateForegroundProvider = 8;
        RegSetValueEx(hKey, TEXT("DownloadRateForegroundProvider"), 0, REG_DWORD, (const BYTE*)&downloadRateForegroundProvider, sizeof(downloadRateForegroundProvider));

        std::cout << "Delivery Optimization keys updated successfully.\n";
        RegCloseKey(hKey);
    }
    else {
        std::cout << "Registry key not found.\n";
    }
}

void TerminateXboxProcesses() {
    const std::string processName = "Xbox";
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to create snapshot.\n";
        return;
    }

    bool hasXboxProcesses = false;
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (strstr(pe.szExeFile, processName.c_str())) {
                hasXboxProcesses = true;
                SetConsoleColor(10, 0);
                std::cout << pe.szExeFile << " (PID: ";
                SetConsoleColor(12, 0);
                std::cout << pe.th32ProcessID << ")\n";
                SetConsoleColor(15, 0);
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    if (!hasXboxProcesses) {
        std::cout << "No Xbox processes found.\n";
        CloseHandle(hSnapshot);
        return;
    }

    std::cout << "\nDo you want to terminate all Xbox processes? (YES to confirm): ";
    std::string input;
    std::getline(std::cin, input);

    if (input == "YES") {
        if (Process32First(hSnapshot, &pe)) {
            do {
                if (strstr(pe.szExeFile, processName.c_str())) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                    if (hProcess != NULL) {
                        TerminateProcess(hProcess, 0);
                        std::cout << "Process " << pe.szExeFile << " (PID: " << pe.th32ProcessID << ") terminated.\n";
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32Next(hSnapshot, &pe));
        }
    }
    else {
        std::cout << "Operation cancelled.\n";
    }

    CloseHandle(hSnapshot);
}

void DisableRealTimeMonitoring() {
    HKEY hKey;
    LONG result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Policies\\Microsoft\\Windows Defender\\Real-Time Protection"), 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);

    if (result == ERROR_SUCCESS) {
        DWORD disableMonitoring = 1;
        RegSetValueEx(hKey, TEXT("DisableRealtimeMonitoring"), 0, REG_DWORD, (const BYTE*)&disableMonitoring, sizeof(disableMonitoring));
        std::cout << "Real-time monitoring has been disabled successfully.\n";
        RegCloseKey(hKey);
    }
    else {
        std::cout << "Failed to create registry key.\n";
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    while (true) {
        std::cout << "Select an option:\n";
        std::cout << "[1] Delivery Optimization\n";
        std::cout << "[2] Xbox Process Terminator\n";
        std::cout << "[3] Disable Real-time Monitoring\n";
        std::cout << "[4] Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            UpdateDeliveryOptimizationKeys();
            break;
        case 2:
            TerminateXboxProcesses();
            break;
        case 3:
            DisableRealTimeMonitoring();
            break;
        case 4:
            std::cout << "Exiting...\n";
            return 0;
        default:
            std::cout << "Invalid choice.\n";
            break;
        }

        std::cout << "\nDo you want to return to the main menu? (YES to continue): ";
        std::string input;  
        std::getline(std::cin, input);
        if (input != "YES" && input != "Y" && input != "y" && input != "Yes" && input != "yes") {
            std::cout << "Exiting...\n";
            break;
        }
        system("cls");
    }

    return 0;
}
