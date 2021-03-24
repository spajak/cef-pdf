// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "SchemeHandlerFactory.h"

#include "include/wrapper/cef_stream_resource_handler.h"

namespace cefpdf {

SchemeHandlerFactory::SchemeHandlerFactory()
{}

void SchemeHandlerFactory::Register(CefRefPtr<CefBrowser> browser, CefRefPtr<CefStreamReader> reader)
{
    m_readers.push_back({browser, reader});
}

void SchemeHandlerFactory::Unregister(CefRefPtr<CefBrowser> browser)
{
    base::AutoLock lock_scope(m_lock);

    for (auto it = m_readers.begin(); it != m_readers.end(); ) {
        if (it->browser->IsSame(browser) || it->browser->HasOneRef()) {
            it = m_readers.erase(it);
        } else { ++it; }
    }
}

void SchemeHandlerFactory::UnregisterAll()
{
    m_readers.clear();
}

// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> SchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    DLOG(INFO) << "SchemeHandlerFactory::Create (" << request->GetURL() << ")";

    base::AutoLock lock_scope(m_lock);

    for (auto r: m_readers) {
        if (r.browser->IsSame(browser)) {
            return new CefStreamResourceHandler("text/html",  r.reader);
        }
    }
    return NULL;
}

} // namespace cefpdf
