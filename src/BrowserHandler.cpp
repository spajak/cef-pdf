#include "BrowserHandler.h"
#include "PdfPrintJob.h"
#include "RenderHandler.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {

BrowserHandler::BrowserHandler(CefRefPtr<PdfPrintJob> printJob)
{
    m_job = printJob;
    m_renderHandler = new RenderHandler;
}

void BrowserHandler::Queue(CefRefPtr<PdfPrintJob> printJob)
{
    m_jobsQueue.push(printJob);
}

bool BrowserHandler::Process()
{
    if (m_processCount >= constants::maxProcesses) {
        return false;
    }

    if (m_jobsQueue.empty()) {
        return false;
    }

    ++m_processCount;

    // Create the browser window.
    CefRefPtr<CefRequestContext> context = CefRequestContext::CreateContext(m_contextSettings, NULL);
    CefBrowserHost::CreateBrowser(m_windowInfo, this, "", m_browserSettings, context);

    return true;
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

    auto printJob = m_jobsQueue.front();
    m_jobsQueue.pop();

    browser->GetHost()
        ->GetRequestContext()
        ->RegisterSchemeHandlerFactory(constants::scheme, "", this);

    m_browser->GetMainFrame()->LoadURL(m_job->GetUrl());

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

    m_browser = NULL;
    m_job->OnDone(m_loadError, m_printError);

    --m_browserCount;
    --m_processCount;
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

    if (frame->IsMain()) {
        auto jobContainer = findJob(browser.get());
        jobContainer->Print();
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
        auto jobContainer = findJob(browser.get());
        jobContainer->loadError = errorCode;
    }
}

// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> BrowserHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    auto jobContainer = findJob(browser.get());
    return new ResponseHandler(jobContainer->job->GetContent());
}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void BrowserHandler::PdfPrintJobContainer::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    DCHECK(browser.get());
    printError = !ok;
    browser->GetHost()->CloseBrowser(true);
}

void BrowserHandler::PdfPrintJobContainer::Print()
{
    if (ERR_NONE != loadError) {
        browser->GetHost()->CloseBrowser(true);
    } else {
        browser->GetHost()->PrintToPDF(
            job->GetOutputPath(),
            job->GetCefPdfPrintSettings(),
            this
        );
    }
}

} // namespace cefpdf
