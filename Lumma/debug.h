#pragma once
#include <string>

namespace Debug
{
    extern bool g_consoleInitialized;

    void Initialize();
    void Shutdown();
    void Log(const char* message);
    void Log(const std::string& message);
    void LogError(const char* message);
    void LogError(const std::string& message);
    void LogWarn(const char* message);
    void LogWarn(const std::string& message);
}