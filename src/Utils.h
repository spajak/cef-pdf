#ifndef UTILS_H_
#define UTILS_H_

#include "include/cef_base.h"

#include <cstddef>

struct CIHash
{
    std::size_t operator()(CefString const& s) const;
};

struct CIEqual
{
    bool operator()(const CefString& lhs, const CefString& rhs) const;
};

#endif // UTILS_H_
