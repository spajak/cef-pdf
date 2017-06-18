#include "Common.h"

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <list>
#include <algorithm>
#include <fstream>
#include <regex>
#include <functional>
#include <random>
#include <cstdlib>

#if !defined(OS_WIN)
#include <unistd.h> // getcwd()
#endif

namespace cefpdf {

PageSizesMap pageSizesMap = {
    {"A0", 841,  1189},
    {"A1", 594,   841},
    {"A2", 420,   594},
    {"A3", 297,   420},
    {"A4", 210,   297},
    {"A5", 148,   210},
    {"A6", 105,   148},
    {"A7",  74,   105},
    {"A8",  52,    74},
    {"A9",  37,    52},
    {"A10", 26,    37},

    {"B0",  1000, 1414},
    {"B1",  707,  1000},
    {"B2",  500,   707},
    {"B3",  353,   500},
    {"B4",  250,   353},
    {"B5",  176,   250},
    {"B6",  125,   176},
    {"B7",  88,    125},
    {"B8",  62,     88},
    {"B9",  44,     62},
    {"B10", 31,     44},

    {"C0",  917,  1297},
    {"C1",  648,   917},
    {"C2",  458,   648},
    {"C3",  324,   458},
    {"C4",  229,   324},
    {"C5",  162,   229},
    {"C6",  114,   162},
    {"C7",  81,    114},
    {"C8",  57,     81},
    {"C9",  40,     57},
    {"C10", 28,     40},

    {"Letter",  216, 279},
    {"Legal",   216, 356},
    {"Tabloid", 279, 432},
    {"Ledger",  432, 279},

    {"Junior Legal",      127, 203},
    {"Half Letter",       140, 216},
    {"Government Letter", 203, 267},
    {"Government Legal",  216, 330},

    {"ANSI A", 216,  279},
    {"ANSI B", 279,  432},
    {"ANSI C", 432,  559},
    {"ANSI D", 559,  864},
    {"ANSI E", 864, 1118},

    {"2R",  64,   89},
    {"3R",  89,  127},
    {"4R",  102, 152},
    {"5R",  127, 178},
    {"6R",  152, 203},
    {"8R",  203, 254},
    {"10R", 254, 305},
    {"11R", 279, 356},
    {"12R", 305, 381}
};

std::string strtolower(std::string s)
{
    std::transform(
        s.begin(),
        s.end(),
        s.begin(),
        [](unsigned char c) { return std::tolower(c); }
    );

    return s;
}

void parseCustomPageSize(PageSize& pageSize, const std::string& str)
{
    std::string input = str + "x";
    enum State { Pending = 0, Width, Height };
    int state = State::Pending;
    std::string value;
    bool hasValue = false;

    for (std::string::const_iterator i = input.begin(); i != input.end(); ++i) {
        char c = *i;
        if (0 != std::isdigit(c)) {
            value.push_back(c);
        } else if ('x' == c || 0 != std::isspace(c)) {
            if (value.empty()) {
                continue;
            }

            int v = std::atoi(value.c_str());
            switch (++state) {
                case State::Width:
                    pageSize.width = pageSize.height = v;
                    break;
                case State::Height:
                    pageSize.height = v;
                    break;
                default:
                    throw std::string("Too many values in page size");
            }

            hasValue = true;
            value.erase();
        } else {
            throw std::string("Invalid character \"") + std::string(1, c) + std::string("\" in page size");
        }
    }

    if (!hasValue) {
        throw std::string("Invalid page size format: \"") + str + std::string("\"");
    }
}

PageSize getPageSize(const CefString& str)
{
    std::string lhs = strtolower(str.ToString());
    PageSizesMap::const_iterator it;

    for (it = pageSizesMap.begin(); it != pageSizesMap.end(); ++it) {
        std::string rhs = strtolower(it->name);
        if (lhs == rhs) {
            return *it;
        }
    }

    PageSize pageSize;
    pageSize.name = "Custom";
    parseCustomPageSize(pageSize, lhs);
    return pageSize;
}

void parseCustomPageMargin(PageMargin& pageMargin, const std::string& str)
{
    std::string input = str + "+";
    enum State { Pending = 0, Top, Right, Bottom, Left };
    int state = State::Pending;
    std::string value;
    bool hasValue = false;

    for (std::string::const_iterator i = input.begin(); i != input.end(); ++i) {
        char c = *i;
        if (0 != std::isdigit(c)) {
            value.push_back(c);
        } else if ('+' == c || 0 != std::isspace(c)) {
            if (value.empty()) {
                continue;
            }

            int v = std::atoi(value.c_str());
            switch (++state) {
                case State::Top:
                    pageMargin.top = pageMargin.right = pageMargin.bottom = pageMargin.left = v;
                    break;
                case State::Right:
                    pageMargin.right = pageMargin.left = v;
                    break;
                case State::Bottom:
                    pageMargin.bottom = v;
                    break;
                case State::Left:
                    pageMargin.left = v;
                    break;
                default:
                    throw std::string("Too many values in margin");
            }

            hasValue = true;
            value.erase();
        } else {
            throw std::string("Invalid character \"") + std::string(1, c) + std::string("\" in margin");
        }
    }

    if (!hasValue) {
        throw std::string("Invalid margin format: \"") + str + std::string("\"");
    }
}

PageMargin getPageMargin(const CefString& str)
{
    PageMargin pageMargin;

    std::string input = strtolower(str.ToString());

    if ("default" == input) {
        pageMargin.type = PDF_PRINT_MARGIN_DEFAULT;
    } else if ("minimum" == input) {
        pageMargin.type = PDF_PRINT_MARGIN_MINIMUM;
    } else if ("none" == input) {
        pageMargin.type = PDF_PRINT_MARGIN_NONE;
    } else { // Custom
        pageMargin.type = PDF_PRINT_MARGIN_CUSTOM;
        parseCustomPageMargin(pageMargin, input);
    }

    return pageMargin;
}

std::chrono::microseconds::rep microtime()
{
    auto tt = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(tt).count();
}

std::string pathToUri(const std::string& path)
{
    std::string uri = path;

#if defined(OS_WIN)
    std::regex re("^[a-z]:\\\\", std::regex_constants::icase);
    if (!std::regex_search(uri, re, std::regex_constants::match_continuous)) {
        uri = getCurrentWorkingDirectory() + std::string("\\") + uri;
    }

    // De-windows the path
    std::replace(uri.begin(), uri.end(), '\\', '/');
    uri = std::string("/") + uri;
#else
    if (uri.front() != '/') {
        uri = getCurrentWorkingDirectory() + std::string("/") + uri;
    }
#endif // OS_WIN

    return std::string("file://") + uri;
}

std::string getCurrentWorkingDirectory()
{
#if defined(OS_WIN)
    wchar_t lpBuffer[MAX_PATH];
    ::GetCurrentDirectoryW(MAX_PATH, lpBuffer);
    char result[2*MAX_PATH];
    std::wcstombs(result, lpBuffer, sizeof(result));
    return result;
#else
    char result[2*4096];
    return getcwd(result, 2*4096);
#endif // OS_WIN
}

std::string getTempDirectory()
{
#if defined(OS_WIN)
    wchar_t lpBuffer[MAX_PATH];
    ::GetTempPathW(MAX_PATH, lpBuffer);
    char result[2*MAX_PATH];
    std::wcstombs(result, lpBuffer, sizeof(result));
    return result;
#else
    const char* vars[4] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};
    for (int i = 0; i < 4; ++i) {
        char* val = std::getenv(vars[i]);
        if (val) {
            return std::string(val) + "/";
        }
    }

    return "/tmp/";
#endif // OS_WIN
}

std::string reserveTempFile()
{
    std::string letters = "9876543210ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(letters.begin(), letters.end(), g);
    std::string path = constants::temp + "cef_" + letters + ".pdf";

    std::ifstream file;
    file.open(path);
    bool isGood = file.good();
    file.close();

    return isGood ? reserveTempFile() : path;
}

std::string loadTempFile(const std::string& path, bool remove)
{
    std::string content;
    std::ifstream output;

    output.open(path, std::ifstream::binary);
    if (output.good()) {
        content.assign((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());
        output.close();
        if (remove) {
            deleteTempFile(path);
        }

        return content;
    }

    throw std::string("Cannot open file: \"") + path + std::string("\"");
}

bool deleteTempFile(const std::string& path)
{
    return 0 == std::remove(path.c_str());
}

} // namespace cefpdf
