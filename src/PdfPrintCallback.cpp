#include "PdfPrintCallback.h"

PdfPrintCallback::PdfPrintCallback(CefRefPtr<CefBrowser> browser)
{
    m_browser = browser;
}

void PdfPrintCallback::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    DCHECK(m_browser.get());

    m_browser->GetHost()->CloseBrowser(false);
    m_browser = NULL;
}
