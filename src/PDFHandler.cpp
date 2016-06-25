#include "PDFHandler.h"
#include <iostream>
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

PDFHandler::PDFHandler() {}


CefRefPtr<CefLifeSpanHandler> PDFHandler::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> PDFHandler::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefRenderHandler> PDFHandler::GetRenderHandler()
{
    return this;
}

void PDFHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    this->_browser = browser;
}

void PDFHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (browser == this->_browser) {
        this->_browser = NULL;
        CefQuitMessageLoop();
    }
}

void PDFHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    CefPdfPrintSettings pdfSettings;
    pdfSettings.backgrounds_enabled = true;

    browser->GetHost()->PrintToPDF("test.pdf", pdfSettings, NULL);

    std::cout << "OnLoadEnd: " << browser->GetMainFrame()->GetURL().ToString() << std::endl;
}

void PDFHandler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED) {
        return;
    }

    // TODO: Display a load error message.
}

bool PDFHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = 1920;
    rect.height = 1080;
    return true;
}

void PDFHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}