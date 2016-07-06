#ifndef BROWSER_HANDLER_H_
#define BROWSER_HANDLER_H_

#include "include/cef_client.h"
#include "include/cef_browser.h"

class BrowserHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefLoadHandler,
                       public CefPdfPrintCallback
{
    public:

    BrowserHandler(
        const CefString& url,
        const CefString& output,
        CefPdfPrintSettings pdfSettings
    );

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) OVERRIDE;

    // CefPdfPrintCallback methods:
    virtual void OnPdfPrintFinished(const CefString& path, bool ok);

    private:

    CefRefPtr<CefBrowser> m_browser;
    int m_browserCount = 0;
    CefString m_url;
    CefString m_output;
    CefPdfPrintSettings m_pdfSettings;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

#endif // BROWSER_HANDLER_H_
