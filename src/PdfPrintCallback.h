#ifndef PDF_PRINT_CALLBACK_H_
#define PDF_PRINT_CALLBACK_H_

#include "JobsManager.h"

#include "include/cef_browser.h"

namespace cefpdf {

class PdfPrintCallback : public CefPdfPrintCallback
{
    public:

    PdfPrintCallback(CefRefPtr<JobsManager> manager, CefRefPtr<CefBrowser> browser);

    // CefPdfPrintCallback methods:
    void OnPdfPrintFinished(const CefString& path, bool ok) override;

    private:

    CefRefPtr<JobsManager> m_manager;
    CefRefPtr<CefBrowser> m_browser;

    IMPLEMENT_REFCOUNTING(PdfPrintCallback);
};

} // namespace cefpdf

#endif // PDF_PRINT_CALLBACK_H_
