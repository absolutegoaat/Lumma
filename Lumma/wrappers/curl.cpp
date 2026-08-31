#include "pch.h"
#include "curl.h"
#include "../utils.h"

#include <windows.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <array>

// since i want this to be JUST a dll and non dependent i have to rely on windows curl
// all vibecoded because im not spending my time
namespace curl {
    static bool RunHiddenProcess(const std::string& cmdLine, std::string& outResult, DWORD& outExitCode)
    {
        outResult.clear();
        outExitCode = 0;

        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = nullptr;

        HANDLE hReadPipe = nullptr;
        HANDLE hWritePipe = nullptr;
        if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
            return false;
        }

        // Ensure the read handle is not inherited by the child process.
        if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return false;
        }

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
        si.hStdInput = nullptr;

        PROCESS_INFORMATION pi{};

        std::string mutableCmd = cmdLine;

        BOOL ok = CreateProcessA(nullptr, mutableCmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

        CloseHandle(hWritePipe);

        if (!ok) {
            CloseHandle(hReadPipe);
            return false;
        }

        // Drain the pipe.
        char buffer[256];
        DWORD bytesRead = 0;
        while (ReadFile(hReadPipe, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
            outResult.append(buffer, bytesRead);
        }
        CloseHandle(hReadPipe);

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        outExitCode = exitCode;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return true;
    }

    HttpResponse Post(const std::string& url, const std::string& data) {
        std::string cmd = "curl.exe -s -X POST \"" + url + "\" "
            "-H \"Content-Type: application/x-www-form-urlencoded\" "
            "-d \"" + data + "\" "
            "-m 15 "
            "-w \"\\n%{http_code}\"";

        std::string result;
        DWORD exitCode = 0;
        if (!RunHiddenProcess(cmd, result, exitCode)) {
            return { "Failed to launch curl.exe", 0 };
        }

        size_t lastNewline = result.find_last_of('\n');
        std::string body, codeStr;
        if (lastNewline != std::string::npos) {
            body = result.substr(0, lastNewline);
            codeStr = result.substr(lastNewline + 1);
        }
        else {
            codeStr = result;
        }

        long code = 0;
        if (!codeStr.empty()) {
            try { code = std::stol(codeStr); }
            catch (...) { code = 0; }
        }

        if (exitCode != 0) {
            return { "CURL error: process exited with code " + std::to_string(exitCode), code };
        }
        return { body, code };
    }

    std::string PostDocument(const std::string& url, const std::string& path)
    {
        std::string cmd =
            "curl.exe -sS -X POST "
            "\"" + url + "\" "
            "-F \"document=@" + path + "\" "
            "-w \"\\n%{http_code}\"";

        std::string result;
        DWORD exitCode = 0;
        if (!RunHiddenProcess(cmd, result, exitCode)) {
            return "Failed to launch curl.exe";
        }

        if (exitCode != 0) {
            return "CURL error: process exited with code " +
                std::to_string(exitCode);
        }

        // Split body and HTTP status code.
        size_t pos = result.rfind('\n');

        if (pos == std::string::npos) {
            return "Invalid curl response: " + result;
        }

        std::string body = result.substr(0, pos);
        std::string codeStr = result.substr(pos + 1);

        // Remove possible CR from Windows line endings.
        if (!codeStr.empty() && codeStr.back() == '\r') {
            codeStr.pop_back();
        }

        long httpCode = 0;

        try {
            httpCode = std::stol(codeStr);
        }
        catch (...) {
            return "Invalid HTTP status: " + codeStr;
        }

        if (httpCode < 200 || httpCode >= 300) {
            return "HTTP error " + std::to_string(httpCode) +
                ": " + body;
        }

        return body;
    }

    HttpResponse Get(const std::string& url, const std::string& data) {
        std::string fullUrl = url;
        if (!data.empty()) {
            fullUrl += (url.find('?') != std::string::npos ? "&" : "?") + data;
        }

        std::string cmd = "curl.exe -s -X GET \"" + fullUrl + "\" "
            "-m 15 "
            "-w \"\\n%{http_code}\"";

        std::string result;
        DWORD exitCode = 0;
        if (!RunHiddenProcess(cmd, result, exitCode)) {
            return { "Failed to launch curl.exe", 0 };
        }

        size_t lastNewline = result.find_last_of('\n');
        std::string body, codeStr;
        if (lastNewline != std::string::npos) {
            body = result.substr(0, lastNewline);
            codeStr = result.substr(lastNewline + 1);
        }
        else {
            codeStr = result;
        }

        long code = 0;
        if (!codeStr.empty()) {
            try { code = std::stol(codeStr); }
            catch (...) { code = 0; }
        }

        if (exitCode != 0) {
            return { "CURL error: process exited with code " + std::to_string(exitCode), code };
        }
        return { body, code };
    }
}