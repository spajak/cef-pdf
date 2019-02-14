#ifndef RENDER_HANDLER_H_
#define RENDER_HANDLER_H_

#include "include/cef_render_handler.h"

namespace cefpdf {

class RenderHandler : public CefRenderHandler
{

public:
    RenderHandler();

    void SetViewWidth(int viewWidth);
    void SetViewHeight(int viewHeight);

    // CefRenderHandler methods:
    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

    virtual void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer, int width, int height
    ) override;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RenderHandler)

    int m_viewWidth;
    int m_viewHeight;
   };

} // namespace cefpdf

#endif // RENDER_HANDLER_H_
