#ifndef BROWSER_HANDLER_H_
#define BROWSER_HANDLER_H_

#include "PdfPrintJob.h"

#include "include/cef_client.h"
#include "include/cef_browser.h"

#include <unordered_map>

namespace cefpdf {

class BrowserHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefLoadHandler,
                       public CefPdfPrintCallback
{
    public:

    typedef std::unordered_map<
        int, int
    > Errors;

    BrowserHandler(CefRefPtr<PdfPrintJob> printJob, CefRefPtr<CefRenderHandler> renderHandler);

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler();
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler();
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler();
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler();

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadStart(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame
    ) OVERRIDE;
    virtual void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode
    ) OVERRIDE;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) OVERRIDE;

    // CefPdfPrintCallback methods:
    void OnPdfPrintFinished(const CefString& path, bool ok) OVERRIDE;

    private:

    CefRefPtr<CefBrowser> m_browser;
    CefRefPtr<PdfPrintJob> m_job;
    CefRefPtr<CefRenderHandler> m_renderHandler;

    int m_browserCount = 0;
    Errors m_errors;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

} // namespace cefpdf

#endif // BROWSER_HANDLER_H_
