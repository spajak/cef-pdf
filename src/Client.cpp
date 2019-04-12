#include "Client.h"
#include "Common.h"
#include "SchemeHandlerFactory.h"
#include "PrintHandler.h"
#include "RenderHandler.h"
#include "RenderProcessHandler.h"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

namespace cefpdf {

Client::Client() :
    m_jobManager(new job::Manager()),
    m_pendingBrowsersCount(0),
    m_browsersCount(0),
    m_initialized(false),
    m_contextInitialized(false),
    m_running(false),
    m_stopAfterLastJob(false),
    m_printHandler(new PrintHandler),
    m_renderHandler(new RenderHandler),
    m_renderProcessHandler(new RenderProcessHandler)
{
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = true;

    m_windowInfo.windowless_rendering_enabled = true;

    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(constants::encoding);
    m_browserSettings.plugins = STATE_DISABLED;
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
        m_jobManager->StopAll();
        CefQuitMessageLoop();
    }
}

void Client::AddJob(CefRefPtr<job::Job> job)
{
    m_jobManager->Queue(job);
    CreateBrowsers(1);
}

void Client::CreateBrowsers(unsigned int browserCount)
{
    m_pendingBrowsersCount += browserCount;

    if (!m_contextInitialized) {
        return;
    }

    while (m_pendingBrowsersCount > 0 && m_browsersCount <= constants::maxProcesses) {
        --m_pendingBrowsersCount;
        ++m_browsersCount;
        CefBrowserHost::CreateBrowser(m_windowInfo, this, "", m_browserSettings, NULL);
    }
}

// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

void Client::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme(constants::scheme, CEF_SCHEME_OPTION_STANDARD);
}

void Client::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO)
        << "Client::OnBeforeCommandLineProcessing"
        << " with process_type: " << process_type.ToString()
        << ", command_line: " << command_line->GetCommandLineString().ToString();

    //command_line->AppendSwitch("disable-gpu");
    //command_line->AppendSwitch("disable-gpu-compositing");
    command_line->AppendSwitch("disable-extensions");
    command_line->AppendSwitch("disable-pinch");
};

CefRefPtr<CefRenderProcessHandler> Client::GetRenderProcessHandler()
{
    return m_renderProcessHandler;
}

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

    CefRegisterSchemeHandlerFactory(constants::scheme, "", new SchemeHandlerFactory(m_jobManager));

    CreateBrowsers();
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

    // Assign this browser to the next job. JobsManager will
    // check if there is any queued job
    m_jobManager->Assign(browser);
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
        CefPostDelayedTask(TID_UI, base::Bind(&Client::Stop, this), 50);
    } else {
        CreateBrowsers();
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
        m_jobManager->Process(browser, httpStatusCode);
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
        m_jobManager->Abort(browser, errorCode);
    }
}

// CefRequestHandler methods:
// -------------------------------------------------------------------------
bool Client::OnBeforeBrowse(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
	bool user_gesture,
    bool is_redirect
) {
    DLOG(INFO) << "Client::OnBeforeBrowse";

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

void Client::OnRenderProcessTerminated(
    CefRefPtr<CefBrowser> browser,
    CefRequestHandler::TerminationStatus status
) {
    DLOG(INFO) << "Client::OnRenderProcessTerminated";
}

} // namespace cefpdf
