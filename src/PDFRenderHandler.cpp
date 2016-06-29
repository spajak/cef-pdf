#include "PDFRenderHandler.h"

PDFRenderHandler::PDFRenderHandler() {}

// CefRenderHandler methods:
// -------------------------------------------------------------------------
bool PDFRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height = 0;
    return true;
}

void PDFRenderHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}
