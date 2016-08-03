#include "BrowserHandler.h"
#include "RenderHandler.h"
#include "PdfPrintCallback.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <iostream>

BrowserHandler::BrowserHandler(const CefString& pdfOutput, CefPdfPrintSettings pdfSettings)
{
    m_pdfOutput = pdfOutput;
    m_pdfSettings = pdfSettings;
    m_renderHandler = new RenderHandler;
}

// CefClient methods:
// -------------------------------------------------------------------------
CefRefPtr<CefLifeSpanHandler> BrowserHandler::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> BrowserHandler::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefRenderHandler> BrowserHandler::GetRenderHandler()
{
    return m_renderHandler;
}

// CefLifeSpanHandler methods:
// -------------------------------------------------------------------------
void BrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();

    ++m_browserCount;
}

bool BrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "DoClose";

    CEF_REQUIRE_UI_THREAD();

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void BrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "OnBeforeClose";

    CEF_REQUIRE_UI_THREAD();

    if (--m_browserCount == 0) {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

// CefLoadHandler methods:
// -------------------------------------------------------------------------
void BrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
    DLOG(INFO) << "OnLoadStart" << ", url: " << frame->GetURL().ToString();

    CEF_REQUIRE_UI_THREAD();
}

void BrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    DLOG(INFO)
        << "OnLoadEnd"
        << ", url: " << frame->GetURL().ToString()
        << ", httpStatusCode: " << httpStatusCode;

    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        browser->GetHost()->PrintToPDF(m_pdfOutput, m_pdfSettings, new PdfPrintCallback(browser.get()));

        auto err = m_errors.find(frame->GetURL());
        if (err == m_errors.end()) {
            browser->GetHost()->PrintToPDF(m_pdfOutput, m_pdfSettings, new PdfPrintCallback(browser.get()));
        } else {
            browser->GetHost()->CloseBrowser(false);
        }
    }
}

void BrowserHandler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    DLOG(INFO)
        << "OnLoadError"
        << ", errorCode: " << errorCode
        << ", failedUrl: " << failedUrl.ToString();

    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED) {
        return;
    }

    if (frame->IsMain()) {
        std::cerr << "Error loading: " << failedUrl.ToString() << std::endl;
        m_errors[failedUrl] = errorCode;
    }
}
