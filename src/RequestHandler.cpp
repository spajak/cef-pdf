#include "RequestHandler.h"

#include <regex>

namespace cefpdf {

// CefRequestHandler methods:
// -------------------------------------------------------------------------
bool RequestHandler::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_redirect
) {
    if (m_schemes.empty()) {
        return false;
    }

    std::string regex;

    for (auto s = m_schemes.cbegin(); s != m_schemes.cend(); ++s) {
        if (s != m_schemes.cbegin()) {
            regex += "|";
        }

        regex += *s;
    }

    regex = "^(" + regex + "):";

    std::string url = request->GetURL().ToString();
    // Allow only specified schemes
    std::regex re(regex, std::regex_constants::icase);
    return !std::regex_search(url, re, std::regex_constants::match_continuous);
}

} // namespace cefpdf
