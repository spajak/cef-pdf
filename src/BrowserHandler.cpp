#include "BrowserHandler.h"
#include "PdfPrintJob.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {

BrowserHandler::BrowserHandler(CefRefPtr<PdfPrintJob> printJob, CefRefPtr<CefRenderHandler> renderHandler)
{
    m_job = printJob;
    m_renderHandler = renderHandler;
}

// CefClient methods:
// -----------------------------------------------------------------------------
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
// -----------------------------------------------------------------------------
void BrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();
    DCHECK(!m_browser.get());

    m_browser = browser;
    m_browser->GetMainFrame()->LoadURL(m_job->GetUrl());
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
    DCHECK(m_browser.get());

    m_browser = NULL;
    m_job->OnDone(m_loadError, m_printError);
}

// CefLoadHandler methods:
// -----------------------------------------------------------------------------
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
    DCHECK(m_browser->IsSame(browser));

    if (frame->IsMain()) {
        if (ERR_NONE != m_loadError) {
            m_browser->GetHost()->CloseBrowser(true);
        } else {
            m_browser->GetHost()->PrintToPDF(
                m_job->GetOutputPath(),
                m_job->GetCefPdfPrintSettings(),
                this
            );
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
        m_loadError = errorCode;
    }
}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void BrowserHandler::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    DCHECK(m_browser.get());

    m_printError = !ok;
    m_browser->GetHost()->CloseBrowser(true);
}

} // namespace cefpdf
