#ifndef BROWSER_HANDLER_H_
#define BROWSER_HANDLER_H_

#include "PdfPrintJob.h"

#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_scheme.h"

namespace cefpdf {

class BrowserHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefLoadHandler,
                       public CefSchemeHandlerFactory
{
    public:

    BrowserHandler();

    void Queue(CefRefPtr<PdfPrintJob> printJob);
    bool Process();

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler();
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler();
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

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERRIDE;

    private:

    std::queue<CefRefPtr<PdfPrintJob>> m_jobsQueue;

    class PdfPrintJobContainer : public CefPdfPrintCallback {
        CefRefPtr<PdfPrintJob> job;
        CefRefPtr<CefBrowser> browser;
        CefLoadHandler::ErrorCode loadError = ERR_NONE;
        bool printError = false;
        void Print();
        void OnPdfPrintFinished(const CefString& path, bool ok) OVERRIDE;
    };

    std::vector<PdfPrintJobContainer> m_jobs;

    std::vector<PdfPrintJobContainer>::iterator findJob(CefRefPtr<CefBrowser> browser);

    int m_processCount = 0;
    int m_browserCount = 0;

    CefRefPtr<CefRenderHandler> m_renderHandler;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

} // namespace cefpdf

#endif // BROWSER_HANDLER_H_
