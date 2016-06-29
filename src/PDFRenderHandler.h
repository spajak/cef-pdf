#ifndef PDF_RENDER_HANDLER_H_
#define PDF_RENDER_HANDLER_H_

#include "include/cef_render_handler.h"

class PDFRenderHandler : public CefRenderHandler
{
    public:

    PDFRenderHandler();

    // CefRenderHandler methods:
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;

    virtual void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer, int width, int height
    ) OVERRIDE;

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PDFRenderHandler);
};

#endif // PDF_RENDER_HANDLER_H_
