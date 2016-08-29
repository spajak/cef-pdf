#include "SchemeHandlerFactory.h"
#include "ResponseHandler.h"

namespace cefpdf {

SchemeHandlerFactory::SchemeHandlerFactory(CefRefPtr<JobsManager> jobsManager)
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
    return new ResponseHandler(
        m_jobsManager->GetJobContainer(browser)->job->GetContent()
    );
}

} // namespace cefpdf
