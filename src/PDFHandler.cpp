#include "PDFHandler.h"
#include <iostream>
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"

PDFHandler::PDFHandler() {}

// CefClient methods:
// -------------------------------------------------------------------------
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

// CefLifeSpanHandler methods:
// -------------------------------------------------------------------------
void PDFHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (!m_browser.get()) {
        m_browser = browser;
    }

    m_browserCount++;
}

bool PDFHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void PDFHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (m_browser.get()) {
        if (m_browser->GetIdentifier() == browser->GetIdentifier()) {
            // Free the browser pointer so that the browser can be destroyed.
            m_browser = NULL;
        }

        if (--m_browserCount == 0) {
            // All browser windows have closed. Quit the application message loop.
            CefQuitMessageLoop();
        }
    }
}

// CefSchemeHandlerFactory methods:
// -------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> PDFHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    std::string body = "<h1>Hello World!!!</h1>";

    // Create a stream reader for body.
    CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(body.c_str())),
        body.size()
    );

    return new CefStreamResourceHandler("text/html", stream);
};

// CefPdfPrintCallback methods:
// -------------------------------------------------------------------------
void PDFHandler::OnPdfPrintFinished(const CefString& path, bool ok)
{
    CEF_REQUIRE_UI_THREAD();

    m_browser->GetHost()->CloseBrowser(false);
}

// CefLoadHandler methods:
// -------------------------------------------------------------------------
void PDFHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    CEF_REQUIRE_UI_THREAD();

    CefPdfPrintSettings pdfSettings;
    pdfSettings.backgrounds_enabled = true;

    m_browser->GetHost()->PrintToPDF("test.pdf", pdfSettings, this);
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


// CefRenderHandler methods:
// -------------------------------------------------------------------------
bool PDFHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect.x = 0;
    rect.y = 0;
    rect.width = 0;
    rect.height = 0;
    return true;
}

void PDFHandler::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}