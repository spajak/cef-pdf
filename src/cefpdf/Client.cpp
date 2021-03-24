// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "Client.h"

#include <string>

#include "ClientHandler.h"
#include "job/ContentProvider.h"
#include "job/Loader.h"
#include "job/Printer.h"
#include "Common.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

Client::Client(CefRefPtr<job::Job> job, CefRefPtr<ClientHandler> handler, CefRefPtr<SchemeHandlerFactory> factory) :
    m_job(job),
    m_handler(handler),
    m_renderHandler(new RenderHandler),
    m_schemeHandlerFactory(factory),
    m_status(Client::Status::PENDING),
    m_browser(NULL)
{
    m_windowInfo.windowless_rendering_enabled = true;
    m_windowInfo.SetAsWindowless(NULL);
    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(constants::encoding);
    m_browserSettings.plugins = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
    m_browserSettings.javascript = job->GetJavaScript() ? STATE_ENABLED : STATE_DISABLED;
}

void Client::Start()
{
    DCHECK(!m_browser);
    bool ok = CefBrowserHost::CreateBrowser(m_windowInfo, this, "", m_browserSettings, NULL, NULL);
    DCHECK(ok);
}

void Client::Stop()
{
    DCHECK(m_browser);
    m_browser->GetHost()->CloseBrowser(true);
    m_browser = NULL;
}

void Client::Print()
{
    // Generate file name if empty
    if (m_job->GetOutputPath().empty()) {
        m_job->SetOutputPath(reserveTempFile());
    }
    m_status = Client::Status::PRINTING;
    // Print to PDF
    CefRefPtr<job::Printer> printer = new job::Printer(this, m_browser);
    m_job->accept(printer);
}

Client::Status Client::GetStatus()
{
    return m_status;
}

// CefClient methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefLifeSpanHandler> Client::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> Client::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefRenderHandler> Client::GetRenderHandler()
{
    return m_renderHandler;
}

CefRefPtr<CefRequestHandler> Client::GetRequestHandler()
{
    return this;
}

bool Client::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message
) {
    DLOG(INFO) << "Client::OnProcessMessageReceived";
    CEF_REQUIRE_UI_THREAD();

    return true;
}


// CefLifeSpanHandler methods:
// -----------------------------------------------------------------------------
void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::OnAfterCreated";
    CEF_REQUIRE_UI_THREAD();
    DCHECK(!m_browser);

    m_browser = browser;
    m_status = Client::Status::LOADING;

    CefRefPtr<job::ContentProvider> provider = new job::ContentProvider;
    m_job->accept(provider);
    CefRefPtr<CefStreamReader> reader = provider->GetStreamReader();
    if (reader) {
        m_schemeHandlerFactory->Register(browser, reader);
    }

    // Load URL to print
    CefRefPtr<job::Loader> loader = new job::Loader(m_browser);
    m_job->accept(loader);
    m_handler->OnStarted(m_job);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::DoClose";
    CEF_REQUIRE_UI_THREAD();
    DCHECK(m_browser->IsSame(browser));

    // Allow the close.
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::OnBeforeClose";
    CEF_REQUIRE_UI_THREAD();
    DCHECK(m_browser->IsSame(browser));
    m_schemeHandlerFactory->Unregister(m_browser);
    m_browser = NULL;
    CefPostDelayedTask(TID_UI, base::Bind(&cefpdf::ClientHandler::OnFinished, m_handler, m_job, m_status), 2);
    //m_handler->OnFinished(m_job, m_status);
}

// CefLoadHandler methods:
// -----------------------------------------------------------------------------
void Client::OnLoadStart(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefLoadHandler::TransitionType transition_type
) {
    DLOG(INFO) << "Client::OnLoadStart (" << frame->GetURL() << ")";
    CEF_REQUIRE_UI_THREAD();
}

void Client::OnLoadEnd(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int httpStatusCode
) {
    DLOG(INFO) << "Client::OnLoadEnd (" << httpStatusCode << ", " << frame->GetURL() << ")";
    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        if (!httpStatusCode || (200 <= httpStatusCode && 300 > httpStatusCode)) {
            Print();
        } else {
            m_status = Client::Status::HTTP_ERROR;
            m_job->SetError("Got unexpected HTTP status code: " + std::to_string(httpStatusCode));
            Stop();
        }
    }
}

void Client::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefLoadHandler::ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    DLOG(INFO) << "Client::OnLoadError (" << errorCode << ", " << failedUrl << ")";
    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        m_status = (errorCode == CefLoadHandler::ErrorCode::ERR_ABORTED) ?
            Client::Status::ABORTED : Client::Status::NET_ERROR;
        m_job->SetError(errorText);
        Stop();
    }
}

// CefRequestHandler methods:
// -----------------------------------------------------------------------------
bool Client::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool user_gesture,
    bool is_redirect
) {
    DLOG(INFO) << "Client::OnBeforeBrowse (" << request->GetURL() << ")";

    if (m_schemes.empty()) {
        return false;
    }

    std::string url = request->GetURL().ToString();

    for (auto s = m_schemes.cbegin(); s != m_schemes.cend(); ++s) {
        if (matchScheme(url, *s)) {
            return false;
        }
    }

    return true;
}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void Client::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO) << "Client::OnPdfPrintFinished";
    DCHECK(m_browser);

    if (!ok) {
        m_status = Client::Status::PRINT_ERROR;
        m_job->SetError("Printing to \"" + path.ToString() + "\" failed");
    } else {
        m_status = Client::Status::SUCCESS;
    }

    Stop();
}

} // namespace cefpdf
