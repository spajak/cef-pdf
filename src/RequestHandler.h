#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include "include/cef_request_handler.h"

#include <string>
#include <set>

namespace cefpdf {

class RequestHandler : public CefRequestHandler
{

public:
    RequestHandler() {}

    void AddAllowedScheme(const std::string& scheme) {
        m_schemes.insert(scheme);
    }

    void ClearAllowedSchemes() {
        m_schemes.clear();
    }

    void RemoveAllowedScheme(const std::string& scheme) {
        auto i = m_schemes.find(scheme);
        if (i != m_schemes.end()) {
            m_schemes.erase(i);
        }
    }

    // CefRequestHandler methods:
    virtual bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_redirect
    ) override;

private:
    std::set<std::string> m_schemes;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RequestHandler)
};

} // namespace cefpdf

#endif // REQUEST_HANDLER_H_
