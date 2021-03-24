// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "App.h"
#include "Common.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

App::App(const CefMainArgs& mainArgs) :
    m_mainArgs(mainArgs),
    m_printHandler(new PrintHandler),
    m_schemeHandlerFactory(new SchemeHandlerFactory),
    m_initialized(false)
{}

void App::Queue(CefRefPtr<job::Job> job)
{
    CefRefPtr<Client> client = new Client(job, this, m_schemeHandlerFactory);
    m_queue.push(client);
    if (m_initialized) {
        ProcessQueue();
    }
}

void App::Run()
{
    DCHECK(!m_initialized);

    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.command_line_args_disabled = true;
    settings.ignore_certificate_errors = true;
    CefInitialize(m_mainArgs, settings, this, NULL);
    CefRunMessageLoop();
    while(!m_queue.empty()) {
        m_queue.pop();
    }
    m_schemeHandlerFactory->UnregisterAll();
    CefClearSchemeHandlerFactories();
    CefShutdown();
    DLOG(INFO) << "::CefShutdown()";
    m_initialized = false;
}

void App::Stop()
{
    CefQuitMessageLoop();
}

CefString App::GetProcessType(CefRefPtr<CefCommandLine> command_line)
{
    if (!command_line->HasSwitch("type")) {
        return "browser";
    }
    return command_line->GetSwitchValue("type");
}

void App::OnFinished(CefRefPtr<job::Job> job, Client::Status status)
{
    DLOG(INFO) << "App::OnFinished";
    if (m_queue.empty()) {
        this->Stop();
    }
}

void App::ProcessQueue()
{
    DCHECK(m_initialized);

    while(!m_queue.empty()) {
        CefRefPtr<Client> c = m_queue.front();
        m_queue.pop();
        c->Start();
    }
}

// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> App::GetBrowserProcessHandler()
{
    return this;
}

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    DLOG(INFO) << "App::OnRegisterCustomSchemes (" << constants::scheme << ")";
    registrar->AddCustomScheme(constants::scheme, CEF_SCHEME_OPTION_STANDARD);
}

void App::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    // Pass additional command-line flags to the browser process.
    if (process_type.empty()) {
        //command_line->AppendSwitch("disable-extensions");
        command_line->AppendSwitch("disable-notifications");
        //command_line->AppendSwitch("disable-gpu");
        //command_line->AppendSwitch("disable-gpu-compositing");
    }
};

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler()
{
    return this;
}

// CefBrowserProcessHandler methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefPrintHandler> App::GetPrintHandler()
{
    return m_printHandler;
}

void App::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "App::OnBeforeChildProcessLaunch (" << GetProcessType(command_line) << ")";
}

void App::OnContextInitialized()
{
    DLOG(INFO) << "App::OnContextInitialized";
    CEF_REQUIRE_UI_THREAD();
    m_initialized = true;

    CefRegisterSchemeHandlerFactory(constants::scheme, "", m_schemeHandlerFactory);
    ProcessQueue();
}

// CefRenderProcessHandler methods:
// -----------------------------------------------------------------------------
void App::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "App::OnContextCreated";
    CEF_REQUIRE_RENDERER_THREAD();
}

void App::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "App::OnContextReleased";
    CEF_REQUIRE_RENDERER_THREAD();
}

} // namespace cefpdf
