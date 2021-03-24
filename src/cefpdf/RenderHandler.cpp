// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "RenderHandler.h"

namespace cefpdf {

RenderHandler::RenderHandler() :
    m_viewWidth(3840),
    m_viewHeight(2160)
{}

void RenderHandler::SetViewWidth(int viewWidth)
{
    DLOG(INFO) << "View width: " << m_viewWidth;
    m_viewWidth = viewWidth;
}

void RenderHandler::SetViewHeight(int viewHeight)
{
    DLOG(INFO) << "View height: " << m_viewHeight;
    m_viewHeight = viewHeight;
}

// CefRenderHandler methods:
// -------------------------------------------------------------------------
void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = m_viewWidth;
    rect.height = m_viewHeight;
}

void RenderHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}

} // namespace cefpdf
