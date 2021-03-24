// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_SCHEME_HANDLER_FACTORY_H_
#define CEFPDF_SCHEME_HANDLER_FACTORY_H_

#include <vector>

#include "include/cef_scheme.h"
#include "include/cef_browser.h"
#include "include/cef_stream.h"
#include "include/base/cef_lock.h"

namespace cefpdf {

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{

public:
    SchemeHandlerFactory();

    void Register(CefRefPtr<CefBrowser>, CefRefPtr<CefStreamReader>);
    void Unregister(CefRefPtr<CefBrowser>);
    void UnregisterAll();

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) override;

private:
    struct Reader {
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<CefStreamReader> reader;
    };
    std::vector<Reader> m_readers;
    base::Lock m_lock;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};

} // namespace cefpdf

#endif // CEFPDF_SCHEME_HANDLER_FACTORY_H_
