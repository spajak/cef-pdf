#include "Client.h"
#include "Common.h"
#include "SchemeHandlerFactory.h"
#include "PrintHandler.h"
#include "RenderHandler.h"
#include "RenderProcessHandler.h"
#include "RequestHandler.h"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_message_router.h"

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
    m_renderProcessHandler(new RenderProcessHandler),
    m_requestHandler(new RequestHandler)
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

void Client::SetRemoteTrigger(bool flag)
{
    m_remoteTrigger = flag;
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

    if (!m_requestHandler->m_messageRouterBrowserSide) {
        CefMessageRouterConfig config;
        config.js_query_function = constants::jsQueryFunction;
        config.js_cancel_function = constants::jsCancelFunction;
        m_requestHandler->m_messageRouterBrowserSide = CefMessageRouterBrowserSide::Create(config);
        m_requestHandler->m_messageRouterBrowserSide->AddHandler(this, true);
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

bool Client::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message
) {
    DLOG(INFO) << "Client::OnProcessMessageReceived";

    CEF_REQUIRE_UI_THREAD();

    m_requestHandler->m_messageRouterBrowserSide->OnProcessMessageReceived(browser, source_process, message);
    return true;
}


// CefLifeSpanHandler methods:
// -----------------------------------------------------------------------------
void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Client::OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();

    if (m_remoteTrigger) {
        CefRefPtr<CefFrame> frame = browser->GetMainFrame();

	// Add convenience functions.
        frame->ExecuteJavaScript(
            "window.cefpdf = { trigger: function () { window."
            + constants::jsQueryFunction
            + "({request: \"trigger\", onSuccess: function () {}, onFailure: function () {}}); }, abort: function () { window."
            + constants::jsQueryFunction
            + "({request: \"abort\", onSuccess: function () {}, onFailure: function () {}}); } };"
            , frame->GetURL()
            , 0
        );
    }

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

    m_requestHandler->m_messageRouterBrowserSide->OnBeforeClose(browser);

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

    if (frame->IsMain() && !m_remoteTrigger) {
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

// CefMessageRouterBrowserSide::Handler methods:
bool Client::OnQuery(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int64 query_id,
    const CefString& request,
    bool persistent,
    CefRefPtr<Callback> callback
) {
    DLOG(INFO) << "Client::OnQuery";

    CEF_REQUIRE_UI_THREAD();

    if (frame->IsMain() && m_remoteTrigger) {
	if (request == "trigger") {
            callback->Success("Processing");
            m_jobManager->Process(browser, 200);
            return true;
	} else if (request == "abort") {
            callback->Failure(ERR_ABORTED, "Aborted");
            m_jobManager->Abort(browser, ERR_ABORTED);
	    return true;
	}
    }
    return false;
}

} // namespace cefpdf
