#include "SchemeHandlerFactory.h"
#include "ResponseHandler.h"
#include "StdInputStreamReader.h"

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
    return new CefStreamResourceHandler("text/html",  CefStreamReader::CreateForHandler(new StdInputStreamReader));
    //return new ResponseHandler();
}

} // namespace cefpdf
