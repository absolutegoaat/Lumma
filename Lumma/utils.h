#pragma once
#include <string>

namespace utils {
    std::string escText(const std::string& text);
    std::string urlEncode(const std::string& text);
    std::string WideToStr(const wchar_t* wide);
    std::wstring WindowsComputerName();
}