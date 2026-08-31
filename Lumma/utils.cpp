#include "pch.h"
#include "utils.h"

#include <windows.h>
#include <string>
#include <iostream>
#include <Shlobj.h>

namespace utils {
    std::string escText(const std::string& text)
    {
        std::string result;
        result.reserve(text.size() * 2);

        const std::string specialChars = "_*[]()~`>#+-=|{}.!";

        for (char c : text)
        {
            if (specialChars.find(c) != std::string::npos)
                result += '\\';

            result += c;
        }

        return result;
    }

    std::string urlEncode(const std::string& text)
    {
        std::string result;
        for (unsigned char c : text)
        {
            if (std::isalnum(c) ||
                c == '-' || c == '_' ||
                c == '.' || c == '~')
            {
                result += static_cast<char>(c);
            }
            else
            {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", c);
                result += buf;
            }
        }
        return result;
    }

    std::string WideToStr(const wchar_t* wide) {
        if (!wide || !*wide) return "";
        int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        if (size <= 0) return "";
        std::string result(static_cast<size_t>(size - 1), '\0');
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
        return result;
    }

    std::wstring WindowsComputerName() {
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(buffer) / sizeof(buffer[0]);

        if (GetComputerNameW(buffer, &size)) {
            return std::wstring(buffer);
        }

        return L"Unknown";
    }
}