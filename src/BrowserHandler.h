#ifndef BROWSER_HANDLER_H_
#define BROWSER_HANDLER_H_

#include "include/cef_client.h"
#include "include/cef_browser.h"

#include <unordered_map>

class BrowserHandler : public CefLifeSpanHandler,
                       public CefLoadHandler
{
    public:

    typedef std::unordered_map<
        int, int
    > Errors;

    BrowserHandler();

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

    private:

    int m_browserCount = 0;
    Errors m_errors;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

#endif // BROWSER_HANDLER_H_
