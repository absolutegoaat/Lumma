#include "pch.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <array>
#include "curl.h"

// since i want this to be JUST a dll and non dependent i have to rely on windows curl
namespace curl {
    struct HttpResponse {
        std::string body;
        long statusCode = 0;
    };

    HttpResponse Post(const std::string& url, const std::string& data) {
        std::string escapedData = data;
        size_t pos = 0;
        while ((pos = escapedData.find('"', pos)) != std::string::npos) {
            escapedData.insert(pos, "\\");
            pos += 2;
        }
        std::string cmd = "curl.exe -s -X POST \"" + url + "\" "
            "-d \"" + escapedData + "\" "
            "-w \"\\n%{http_code}\"";

        std::string result;
        std::array<char, 256> buffer;
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) {
            return { "Failed to launch curl.exe", 0 };
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        int exitCode = _pclose(pipe);

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
        std::array<char, 256> buffer{};

        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) {
            return "Failed to launch curl.exe";
        }

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe)) {
            result += buffer.data();
        }

        int exitCode = _pclose(pipe);

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
}