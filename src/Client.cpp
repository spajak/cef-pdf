#include "Client.h"
#include "Common.h"
#include "SchemeHandlerFactory.h"
#include "PrintHandler.h"
#include "RenderHandler.h"
#include "RequestHandler.h"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

namespace cefpdf {

Client::Client() :
    m_jobsManager(new job::Manager()),
    m_pendingBrowsersCount(0),
    m_browsersCount(0),
    m_initialized(false),
    m_contextInitialized(false),
    m_running(false),
    m_stopAfterLastJob(false),
    m_printHandler(new PrintHandler),
    m_renderHandler(new RenderHandler),
    m_requestHandler(new RequestHandler)
{
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = true;

    m_windowInfo.windowless_rendering_enabled = true;

    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(constants::encoding);
    m_browserSettings.plugins = STATE_DISABLED;
    m_browserSettings.javascript_open_windows = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
}

int Client::ExecuteSubProcess(const CefMainArgs& mainArgs)
{
    return CefExecuteProcess(mainArgs, this, NULL);
}

void Client::Initialize(const CefMainArgs& mainArgs)
{
    DCHECK(!m_initialized);

    m_initialized = true;
    CefInitialize(mainArgs, m_settings, this, NULL);
}

void Client::Shutdown()
{
    DCHECK(m_initialized);

    CefShutdown();
    m_contextInitialized = false;
    m_initialized = false;
}

void Client::Run()
{
    DCHECK(m_initialized);
    DCHECK(!m_running);

    m_running = true;
    CefRunMessageLoop();
    m_running = false;
    Shutdown();
}

void Client::Stop()
{
    DCHECK(m_initialized);

    if (m_running) {
        m_pendingBrowsersCount = 0;
        m_jobsManager->StopAll();
        CefDoMessageLoopWork();
        CefQuitMessageLoop();
    }
}

void Client::AddJob(CefRefPtr<job::Job> job)
{
    m_jobsManager->Queue(job);

    if (!m_contextInitialized) {
        ++m_pendingBrowsersCount;
    } else {
        CreateBrowser();
    }
}

void Client::PostStop()
{
    CefPostDelayedTask(TID_UI, base::Bind(&cefpdf::Client::Stop, this), 50);
}

void Client::PostJob(CefRefPtr<job::Job> job)
{
    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::AddJob, this, job.get()));
}

void Client::CreateBrowser()
{
    DCHECK(m_contextInitialized);

    if (m_browsersCount > constants::maxProcesses) {
        CefPostDelayedTask(TID_UI, base::Bind(&cefpdf::Client::CreateBrowser, this), 15*m_browsersCount);
        return;
    }

    ++m_browsersCount;
    CefBrowserHost::CreateBrowser(m_windowInfo, this, "", m_browserSettings, NULL);
}

// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

void Client::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme(constants::scheme, true, false, false, false, true, false);
}

void Client::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO)
        << "Client::OnBeforeCommandLineProcessing"
        << " with process_type: " << process_type.ToString()
        << ", command_line: " << command_line->GetCommandLineString().ToString();

    command_line->AppendSwitch("disable-gpu");
    command_line->AppendSwitch("disable-gpu-compositing");
};

// CefBrowserProcessHandler methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefPrintHandler> Client::GetPrintHandler()
{
    return m_printHandler;
}

void Client::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO)
        << "Client::OnBeforeChildProcessLaunch"
        << " with command_line: " << command_line->GetCommandLineString().ToString();
}

void Client::OnContextInitialized()
{
    DLOG(INFO) << "Client::OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    m_contextInitialized = true;

    CefRegisterSchemeHandlerFactory(constants::scheme, "", new SchemeHandlerFactory(m_jobsManager));

    while (m_pendingBrowsersCount > 0) {
        --m_pendingBrowsersCount;
        CreateBrowser();
    }
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
    return m_requestHandler;
}

// CefLifeSpanHandler methods:
// -----------------------------------------------------------------------------
void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();

    // Assign this browser to the next job. JobsManager will
    // check if there is any queued job
    m_jobsManager->Assign(browser);
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::DoClose";

    CEF_REQUIRE_UI_THREAD();

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::OnBeforeClose";

    CEF_REQUIRE_UI_THREAD();

    --m_browsersCount;

    if (0 == m_browsersCount && m_stopAfterLastJob) {
        PostStop();
    }
}

// CefLoadHandler methods:
// -----------------------------------------------------------------------------
void Client::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    DLOG(INFO)
        << "Client::OnLoadStart"
        << " with url: " << frame->GetURL().ToString();

    CEF_REQUIRE_UI_THREAD();
}

void Client::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    DLOG(INFO)
        << "Client::OnLoadEnd"
        << " with url: " << frame->GetURL().ToString()
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
        << "Client::OnLoadError"
        << " with errorCode: " << errorCode
        << ", failedUrl: " << failedUrl.ToString();

    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain()) {
        m_jobsManager->Abort(browser, errorCode);
    }
}

} // namespace cefpdf
