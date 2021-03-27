// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "BrowserHandler.h"

#include <string>

#include "Common.h"

#include "include/internal/cef_types.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"

namespace cefpdf {

BrowserHandler::BrowserHandler(CefRefPtr<PrintJobCallback> callback) :
    m_renderHandler(new RenderHandler),
    m_callback(callback),
    m_browsersCounter(0)
{
    m_windowInfo.windowless_rendering_enabled = true;
    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(constants::encoding);
    m_browserSettings.plugins = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
    m_browserSettings.javascript = STATE_DISABLED;
}

void BrowserHandler::Queue(CefRefPtr<PrintJob> job)
{
    job->SetStatus(PrintJob::Status::PENDING);
    m_jobQueue.push(job);
}

void BrowserHandler::Process(bool enableJavaScript)
{
    std::size_t max = m_jobQueue.size();
    if (max > constants::maxProcesses) {
        max = constants::maxProcesses;
    }
    while (m_browsersCounter < max) {
        m_browsersCounter += 1;
        CefBrowserSettings browserSettings;
        browserSettings.Set(m_browserSettings, true);
        browserSettings.javascript = enableJavaScript ? STATE_ENABLED : STATE_DISABLED;
        bool ok = CefBrowserHost::CreateBrowser(m_windowInfo, this, "", browserSettings, NULL, NULL);
        DCHECK(ok);
    }
}

CefRefPtr<PrintJobBrowser> BrowserHandler::FindJobBrowser(CefRefPtr<CefBrowser> browser)
{
    for (auto jb: m_jobBrowsers) {
        if (jb->GetBrowser()->IsSame(browser)) {
            return jb;
        }
    }
    return NULL;
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

CefRefPtr<CefRequestHandler> BrowserHandler::GetRequestHandler()
{
    return this;
}

// CefLifeSpanHandler methods:
// -----------------------------------------------------------------------------
void BrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "BrowserHandler::OnAfterCreated (CefLifeSpanHandler)";
    DLOG(INFO) << "|- Browser ID: " << browser->GetIdentifier();
    CEF_REQUIRE_UI_THREAD();
    DCHECK(!m_jobQueue.empty());

    CefRefPtr<PrintJob> job = m_jobQueue.front();
    m_jobQueue.pop();
    DCHECK(job->GetStatus() == PrintJob::Status::PENDING);
    CefRefPtr<PrintJobBrowser> jobBrowser = new PrintJobBrowser(job, browser);
    m_jobBrowsers.push_back(jobBrowser);
    jobBrowser->Load();
}

bool BrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "BrowserHandler::DoClose (CefLifeSpanHandler)";
    CEF_REQUIRE_UI_THREAD();

    auto jobBrowser = FindJobBrowser(browser);
    DCHECK(jobBrowser);

    // Allow the close.
    return false;
}

void BrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "BrowserHandler::OnBeforeClose (CefLifeSpanHandler)";
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<PrintJob> job;

    for (auto it = m_jobBrowsers.begin(); it != m_jobBrowsers.end(); ++it) {
        if ((*it)->GetBrowser()->IsSame(browser)) {
            job = (*it)->GetPrintJob();
            m_jobBrowsers.erase(it);
            break;
        }
    }

    DCHECK(job);
    DCHECK(job->IsFinished());
    m_browsersCounter -= 1;
    m_callback->OnPrintFinished(job);
}

// CefLoadHandler methods:
// -----------------------------------------------------------------------------
void BrowserHandler::OnLoadStart(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefLoadHandler::TransitionType transition_type
) {
    if (!frame->IsMain()) {
        return;
    }

    DLOG(INFO) << "BrowserHandler::OnLoadStart (CefLoadHandler)";
    DLOG(INFO) << "|- Url: " << frame->GetURL();
}

void BrowserHandler::OnLoadEnd(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int httpStatusCode
) {
    if (!frame->IsMain()) {
        return;
    }

    DLOG(INFO) << "BrowserHandler::OnLoadEnd (CefLoadHandler)";
    DLOG(INFO) << "|- Url: " << frame->GetURL();
    DLOG(INFO) << "|- Status code: " << httpStatusCode;

    auto jobBrowser = FindJobBrowser(browser);
    DCHECK(jobBrowser);

    if (!httpStatusCode || (200 <= httpStatusCode && 300 > httpStatusCode)) {
        jobBrowser->Print();
        m_callback->OnPrintStarted(jobBrowser->GetPrintJob());
    } else {
        jobBrowser->SetError(
            PrintJob::ErrorCode::HTTP,
            "Got unexpected HTTP status code: " + std::to_string(httpStatusCode)
        );
        jobBrowser->CloseBrowser();
    }
}

void BrowserHandler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefLoadHandler::ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    if (!frame->IsMain()) {
        return;
    }

    DLOG(INFO) << "BrowserHandler::OnLoadError (CefLoadHandler)";
    DLOG(INFO) << "|- Url: " << failedUrl;
    DLOG(INFO) << "|- Error code: " << errorCode;

    auto jobBrowser = FindJobBrowser(browser);
    DCHECK(jobBrowser);

    auto jobErrorCode = (errorCode == CefLoadHandler::ErrorCode::ERR_ABORTED) ?
        PrintJob::ErrorCode::ABORTED : PrintJob::ErrorCode::NETWORK;

    jobBrowser->SetError(jobErrorCode, errorText);
    jobBrowser->CloseBrowser();
}

// CefRequestHandler methods:
// -----------------------------------------------------------------------------
bool BrowserHandler::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool user_gesture,
    bool is_redirect
) {
    DLOG(INFO) << "BrowserHandler::OnBeforeBrowse (CefRequestHandler)";
    DLOG(INFO) << "|- Url: " << request->GetURL();
    DLOG(INFO) << "|- Is redirect: " << (is_redirect ? "YES" : "No");

    // false - allow, true - cancel
    return false;
}

// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> BrowserHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    DLOG(INFO) << "BrowserHandler::Create (CefSchemeHandlerFactory)";
    DLOG(INFO) << "|- Url: " << request->GetURL();
    DLOG(INFO) << "|- Scheme: " << scheme_name;

    auto jobBrowser = FindJobBrowser(browser);
    DCHECK(jobBrowser);

    auto reader = jobBrowser->GetPrintJob()->GetStreamReader();
    if (reader) {
        return new CefStreamResourceHandler("text/html",  reader);
    }

    return NULL;
}

} // namespace cefpdf
