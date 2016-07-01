#ifndef PDF_BROWSER_HANDLER_H_
#define PDF_BROWSER_HANDLER_H_

#include "include/cef_client.h"
#include "include/cef_browser.h"

class PDFBrowserHandler : public CefClient,
                          public CefLifeSpanHandler,
                          public CefLoadHandler,
                          public CefPdfPrintCallback
{
    public:

    enum Method { WWW, FILE, STRING };

    PDFBrowserHandler();

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefPdfPrintCallback methods:
    virtual void OnPdfPrintFinished(const CefString& path, bool ok);

    // CefLoadHandler methods:
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) OVERRIDE;

    private:

    CefRefPtr<CefBrowser> m_browser;
    int m_browserCount = 0;

    Method m_method;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PDFBrowserHandler);
};

#endif // PDF_BROWSER_HANDLER_H_
