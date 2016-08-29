#include "PdfPrintCallback.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

PdfPrintCallback::PdfPrintCallback(CefRefPtr<JobsManager> manager, CefRefPtr<CefBrowser> browser)
    : m_manager(manager), m_browser(browser) {}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void PdfPrintCallback::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    m_manager->OnFinish(m_browser, path, ok);
}

} // namespace cefpdf