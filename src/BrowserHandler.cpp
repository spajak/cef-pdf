#include "BrowserHandler.h"
#include "PdfPrintJob.h"
#include "RenderHandler.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {

BrowserHandler::BrowserHandler(CefRefPtr<PdfPrintJob> printJob)
{
    m_job = printJob;

}









// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> BrowserHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    auto jobContainer = findJob(browser.get());
    return new ResponseHandler(jobContainer->job->GetContent());
}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void BrowserHandler::PdfPrintJobContainer::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    DCHECK(browser.get());
    printError = !ok;
    browser->GetHost()->CloseBrowser(true);
}

void BrowserHandler::PdfPrintJobContainer::Print()
{
    if (ERR_NONE != loadError) {
        browser->GetHost()->CloseBrowser(true);
    } else {
        browser->GetHost()->PrintToPDF(
            job->GetOutputPath(),
            job->GetCefPdfPrintSettings(),
            this
        );
    }
}

} // namespace cefpdf
