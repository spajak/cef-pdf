#ifndef PDF_PRINT_CALLBACK_H_
#define PDF_PRINT_CALLBACK_H_

#include "include/cef_browser.h"

class PdfPrintCallback : public CefPdfPrintCallback
{
    public:

    PdfPrintCallback(CefRefPtr<CefBrowser> browser);

    // CefPdfPrintCallback methods:
    virtual void OnPdfPrintFinished(const CefString& path, bool ok);

    private:

    CefRefPtr<CefBrowser> m_browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PdfPrintCallback);
};

#endif // PDF_PRINT_CALLBACK_H_
