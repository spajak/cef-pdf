// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_RENDER_HANDLER_H_
#define CEFPDF_RENDER_HANDLER_H_

#include "include/cef_render_handler.h"

namespace cefpdf {

class RenderHandler : public CefRenderHandler
{

public:
    RenderHandler();

    void SetViewWidth(int viewWidth);
    void SetViewHeight(int viewHeight);

    // CefRenderHandler methods:
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

    virtual void OnPaint(
        CefRefPtr<CefBrowser> browser,
        CefRenderHandler::PaintElementType type,
        const CefRenderHandler::RectList& dirtyRects,
        const void* buffer, int width, int height
    ) override;

private:
    int m_viewWidth;
    int m_viewHeight;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RenderHandler);
};

} // namespace cefpdf

#endif // CEFPDF_RENDER_HANDLER_H_
