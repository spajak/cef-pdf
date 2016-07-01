#include "RenderHandler.h"

RenderHandler::RenderHandler() {}

// CefRenderHandler methods:
// -------------------------------------------------------------------------
bool RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height = 0;
    return true;
}

void RenderHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}
