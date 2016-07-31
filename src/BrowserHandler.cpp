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

    m_browsers.push_back(browser);
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

    BrowserList::iterator it = m_browsers.begin();
    for (; it != m_browsers.end(); ++it) {
        if ((*it)->IsSame(browser)) {
            m_browsers.erase(it);
            break;
        }
    }

    if (m_browsers.empty()) {
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
    DLOG(INFO) << "OnLoadEnd" << ", httpStatusCode: " << httpStatusCode;

    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        // Save page to PDF
        browser->GetHost()->PrintToPDF(m_pdfOutput, m_pdfSettings, new PdfPrintCallback(browser.get()));
    }
}

void BrowserHandler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    CefString _errorText(errorText.empty() ? "None" : errorText);

    DLOG(INFO)
        << "OnLoadError"
        << ", errorCode: " << errorCode
        << ", errorText: " << _errorText.ToString()
        << ", failedUrl: " << failedUrl.ToString();

    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED) {
        return;
    }

    if (frame->IsMain()) {
        std::cerr << "Error: " << _errorText.ToString() << " " << failedUrl.ToString() << std::endl;
        //browser->GetHost()->CloseBrowser(false);
    }
}
