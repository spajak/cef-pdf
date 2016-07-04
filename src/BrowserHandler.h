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

    enum class Orientation { PORTRAIT, LANSCAPE };

    BrowserHandler();

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

    struct PaperSize { int width; int height; };

    std::map<CefString, PaperSize> m_paperSizes = {
        {"A4", {210000, 297000}},
        {"A5", {148000, 210000}}
    };

    CefRefPtr<CefBrowser> m_browser;
    int m_browserCount = 0;

    Orientation m_orientation = Orientation::PORTRAIT;
    CefString m_paperSize = "A4";




    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

#endif // BROWSER_HANDLER_H_
