#include "Utils.h"

#include <string>
#include <functional>
#include <algorithm>
#include <cctype>

std::size_t CIHash::operator()(CefString const& s) const
{
    std::string str = s.ToString();
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return std::hash<std::string>()(str);
}

bool CIEqual::operator()(const CefString& lhs, const CefString& rhs) const
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (int i = 0; i < lhs.length(); ++i) {
        if (std::tolower(lhs.ToString()[i]) != std::tolower(rhs.ToString()[i])) {
            return false;
        }
    }

    return true;
};


CefString GetProgramName(CefString path)
{
    std::string program = path.ToString();

    // Remove directory if present.
    // Do this before extension removal in case directory has a period character.
    const std::size_t s = program.find_last_of("\\/");
    if (std::string::npos != s) {
        program.erase(0, s + 1);
    }

    // Remove extension if present.
    const std::size_t e = program.rfind('.');
    if (std::string::npos != e) {
        program.erase(e);
    }

    return program;
}
