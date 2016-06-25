#ifndef PDFHANDLER_H
#define PDFHANDLER_H

#include "include/cef_client.h"

class PDFHandler : public CefClient,
                   public CefLifeSpanHandler,
                   public CefLoadHandler,
                   public CefRenderHandler
{
    public:

    PDFHandler();

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) OVERRIDE;

    // CefRenderHandler methods:
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;

    virtual void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer, int width, int height
    ) OVERRIDE;

    private:

    CefRefPtr<CefBrowser> _browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PDFHandler);
};

#endif // PDFHANDLER_H
