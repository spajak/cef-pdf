#include "RenderHandler.h"

namespace cefpdf {

RenderHandler::RenderHandler() 
{
    m_viewWidth = 128;
    m_viewHeight = 128;
}

void RenderHandler::SetViewWidth(int viewWidth)
{
    m_viewWidth = viewWidth;
    DLOG(INFO) << "View width: " << m_viewWidth;
}

void RenderHandler::SetViewHeight(int viewHeight)
{
    m_viewHeight = viewHeight;
    DLOG(INFO) << "View height: " << m_viewHeight;
}

// CefRenderHandler methods:
// -------------------------------------------------------------------------
bool RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = m_viewWidth;
    rect.height = m_viewHeight;
    return true;
}

void RenderHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}

} // namespace cefpdf
