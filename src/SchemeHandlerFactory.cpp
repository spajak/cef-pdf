#include "SchemeHandlerFactory.h"

#include "include/wrapper/cef_stream_resource_handler.h"

namespace cefpdf {

SchemeHandlerFactory::SchemeHandlerFactory(CefRefPtr<job::Manager> jobsManager)
{
    m_jobsManager = jobsManager;
}

// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> SchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    auto streamReader = m_jobsManager->GetStreamReader(browser);

    if (!streamReader.get()) {
        return nullptr;
    }

    return new CefStreamResourceHandler("text/html",  streamReader);
}

} // namespace cefpdf
