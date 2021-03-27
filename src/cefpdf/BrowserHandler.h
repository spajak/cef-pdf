// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_BROWSER_HANDLER_H_
#define CEFPDF_BROWSER_HANDLER_H_

#include <set>
#include <vector>
#include <queue>
#include <string>

#include "PrintJob.h"
#include "PrintJobBrowser.h"
#include "PrintJobCallback.h"
#include "RenderHandler.h"
#include "StdInputReader.h"

#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_scheme.h"

namespace cefpdf {

class BrowserHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefLoadHandler,
                       public CefRequestHandler,
                       public CefSchemeHandlerFactory
{

public:
    typedef std::queue<CefRefPtr<PrintJob>> PrintJobQueue;
    typedef std::vector<CefRefPtr<PrintJobBrowser>> PrintJobBrowsers;

    explicit BrowserHandler(CefRefPtr<PrintJobCallback>);

    BrowserHandler(const BrowserHandler&) = delete;
    BrowserHandler& operator=(const BrowserHandler&) = delete;

    void Queue(CefRefPtr<PrintJob>);
    void Process(bool enableJavaScript = false);

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods:
    virtual void OnLoadStart(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefLoadHandler::TransitionType transition_type
    ) override;
    virtual void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode
    ) override;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefLoadHandler::ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) override;

    // CefRequestHandler methods:
    virtual bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool user_gesture,
        bool is_redirect
    ) override;

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) override;

private:
    CefRefPtr<RenderHandler> m_renderHandler;
    CefRefPtr<PrintJobCallback> m_callback;
    std::size_t m_browsersCounter;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;

    PrintJobQueue m_jobQueue;
    PrintJobBrowsers m_jobBrowsers;

    CefRefPtr<PrintJobBrowser> FindJobBrowser(CefRefPtr<CefBrowser> browser);

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

} // namespace cefpdf

#endif // CEFPDF_BROWSER_HANDLER_H_
