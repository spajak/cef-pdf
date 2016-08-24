#include "Common.h"

#include <list>
#include <cctype>
#include <algorithm>

namespace cefpdf {

std::list<PageSize> pageSizesMap = {
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
    auto tolower = [](unsigned char c) { return std::tolower(c); };
    std::transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}

} // namespace cefpdf