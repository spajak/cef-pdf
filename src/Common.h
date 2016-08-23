#ifndef COMMON_H_
#define COMMON_H_

#include "include/cef_base.h"

#include <cstdlib>
#include <string>
#include <unordered_map>

namespace cefpdf {

namespace constants {
    const std::string version = "0.1.5";
    const std::string scheme = "cefpdf";
}

struct PageSize {
    int width, height;
};

struct PageMargin {
    int top, right, bottom, left;
};

enum struct PageOrientation {
    PORTRAIT, LANDSCAPE
};

struct CIHash
{
    std::size_t operator()(CefString const& s) const;
};

struct CIEqual
{
    bool operator()(const CefString& lhs, const CefString& rhs) const;
};

typedef std::unordered_map<
    CefString,
    PageSize,
    CIHash, CIEqual
> PageSizesMap;

extern PageSizesMap pageSizesMap;

} // namespace cefpdf

#endif // COMMON_H_
