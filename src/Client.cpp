#include "Client.h"
#include "Common.h"
#include "SchemeHandlerFactory.h"
#include "PrintHandler.h"
#include "RenderHandler.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace cefpdf {

Client::Client(bool stopAfterLastJob) :
    m_storage(new Storage),
    m_jobsManager(new job::Manager()),
    m_processCount(0),
    m_initialized(false),
    m_stopAfterLastJob(stopAfterLastJob),
    m_printHandler(new PrintHandler),
    m_renderHandler(new RenderHandler)
{
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = true;
    m_settings.uncaught_exception_stack_size = 20;

    m_windowInfo.windowless_rendering_enabled = true;
    m_windowInfo.transparent_painting_enabled = false;

    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(constants::encoding);
    m_browserSettings.javascript_open_windows = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
    m_browserSettings.plugins = STATE_DISABLED;
}

void Client::Run()
{
    DCHECK(!m_initialized);
    CefMainArgs mainArgs;
    CefInitialize(mainArgs, m_settings, this, NULL);
    CefRunMessageLoop();
    CefShutdown();
    m_initialized = false;
}

void Client::Stop()
{
    DCHECK(m_initialized);
    CefQuitMessageLoop();
    m_initialized = false;
}

void Client::PostJob(CefRefPtr<job::Job> job)
{
    m_jobsQueue.push(job);
    if (m_initialized) {
        ProcessJobsQueue();
    }
}

void Client::ProcessJobsQueue()
{
    CEF_REQUIRE_UI_THREAD();

    while (!m_jobsQueue.empty() && m_processCount < constants::maxProcesses) {
        auto job = m_jobsQueue.front();
        if (job->GetOutputPath().empty()) {
            job->SetOutputPath(m_storage->Reserve());
        }

        m_jobsManager->Queue(job);
        m_jobsQueue.pop();
        ++m_processCount;

        // Create the browser window.
        CefBrowserHost::CreateBrowser(m_windowInfo, this, "", m_browserSettings, NULL);
    }
}

// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

void Client::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme(constants::scheme, true, false, false);
}

// CefBrowserProcessHandler methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefPrintHandler> Client::GetPrintHandler()
{
    return m_printHandler;
}

void Client::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "OnBeforeChildProcessLaunch: " << command_line->GetCommandLineString().ToString() ;
}

void Client::OnContextInitialized()
{
    DLOG(INFO) << "OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    CefRegisterSchemeHandlerFactory(constants::scheme, "", new SchemeHandlerFactory(m_jobsManager));
    m_initialized = true;
    ProcessJobsQueue();
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

// CefLifeSpanHandler methods:
// -----------------------------------------------------------------------------
void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();

    // Assign this browser to the next job. JobsManager will
    // check if there is any queued job
    m_jobsManager->Assign(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "DoClose";

    CEF_REQUIRE_UI_THREAD();

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "OnBeforeClose";

    CEF_REQUIRE_UI_THREAD();

    --m_processCount;
    ProcessJobsQueue();

    if (0 == m_processCount && m_stopAfterLastJob) {
        CefPostTask(TID_UI, base::Bind(&cefpdf::Client::Stop, this));
    }
}

// CefLoadHandler methods:
// -----------------------------------------------------------------------------
void Client::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    DLOG(INFO) << "OnLoadStart" << ", url: " << frame->GetURL().ToString();

    CEF_REQUIRE_UI_THREAD();
}

void Client::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    DLOG(INFO)
        << "OnLoadEnd"
        << ", url: " << frame->GetURL().ToString()
        << ", httpStatusCode: " << httpStatusCode;

    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        m_jobsManager->Process(browser, httpStatusCode);
    }
}

void Client::OnLoadError(
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
        m_jobsManager->SetError(browser, errorCode);
    }
}

} // namespace cefpdf
