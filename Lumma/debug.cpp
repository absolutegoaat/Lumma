#include "pch.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <consoleapi.h>
#include <consoleapi2.h>
#include <Windows.h>

/*
    DEBUG CONSOLE
    easy copy and paste code 
	if you have log messages and want to remove the debug console, just comment out the Debug::Initialize() and Debug::Shutdown() calls in your init.cpp file
    this is hooked to the application so when the console is closed it will close the entire app
*/

namespace Debug
{
    bool g_consoleInitialized = false;
    void Initialize()
    {
        if (g_consoleInitialized)
            return;
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);
        // enable ANSI colors
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleTitleA("lumma");
        g_consoleInitialized = true;

        Log("Debug console initialized");
    }
    void Shutdown()
    {
        if (!g_consoleInitialized)
            return;
        Log("Debug shutting down");
        FreeConsole();
        g_consoleInitialized = false;
    }

    void Log(const char* message)
    {
        if (!g_consoleInitialized)
            return;
        std::cout << "[INFO] " << message << std::endl;
    }
    void Log(const std::string& message)
    {
        Log(message.c_str());
    }

    void LogError(const char* message)
    {
        if (!g_consoleInitialized)
            return;
        std::cerr << "\x1b[31m[ERROR]\x1b[0m " << message << std::endl;
    }
    void LogError(const std::string& message)
    {
        LogError(message.c_str());
    }

    void LogWarn(const char* message) {
        if (!g_consoleInitialized) return;
        std::cout << "\033[38;2;255;165;0m[WARN]\033[0m " << message << std::endl;
    }
    void LogWarn(const std::string& message) {
        LogWarn(message.c_str());
    }
}