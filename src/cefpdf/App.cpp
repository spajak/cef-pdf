// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "App.h"
#include "Common.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

App::App(const CefMainArgs& mainArgs, bool enableJavaScript) :
    m_mainArgs(mainArgs),
    m_browserHandler(new BrowserHandler(this)),
    m_linuxPrintHandler(new LinuxPrintHandler),
    m_initialized(false)
{}

void App::AddPrintJob(CefRefPtr<PrintJob> job)
{
    m_browserHandler->Queue(job);
}

// JobPrintCallback methods:
void App::OnPrintStarted(CefRefPtr<PrintJob> job)
{
    DLOG(INFO) << "App::OnJobPrintStarted";
}

void App::OnPrintFinished(CefRefPtr<PrintJob> job)
{
    job->Release();
    DLOG(INFO) << "App::OnJobPrintFinished";
    this->Stop();
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
    CefClearSchemeHandlerFactories();
    CefShutdown();

    m_initialized = false;
}

void App::Stop()
{
    CefPostDelayedTask(TID_UI, base::Bind(&CefQuitMessageLoop), 10);
}

CefRefPtr<CefStreamReader> App::CreateStreamReaderFromString(const CefString& content)
{
    auto str = content.ToString();
    if (str.empty()) {
        str = "<!DOCTYPE html>";
    }

    CefRefPtr<CefStreamReader> reader = CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(str.c_str())),
        str.size()
    );

    return reader;
}

CefRefPtr<CefStreamReader> App::CreateStreamReaderFromStdInput()
{
    return CefStreamReader::CreateForHandler(new StdInputReader);
}

CefRefPtr<CefStreamReader> App::CreateStreamReaderFromFile(const CefString& fileName)
{
    return CefStreamReader::CreateForFile(fileName);
}

CefString App::GetProcessType(CefRefPtr<CefCommandLine> command_line)
{
    if (!command_line->HasSwitch("type")) {
        return "browser";
    }
    return command_line->GetSwitchValue("type");
}

// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> App::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler()
{
    return this;
}

void App::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "App::OnBeforeCommandLineProcessing (CefApp)";
    DLOG(INFO) << "|- Process type: " << (process_type.empty() ? "browser" : process_type);

    // Pass additional command-line flags to the process.
    if (!process_type.empty()) {
        //command_line->AppendSwitch("disable-extensions");
        //command_line->AppendSwitch("disable-notifications");
        //command_line->AppendSwitch("disable-gpu");
        //command_line->AppendSwitch("disable-gpu-compositing");
    }
};

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    DLOG(INFO) << "App::OnRegisterCustomSchemes (CefApp)";
    registrar->AddCustomScheme(constants::scheme, CEF_SCHEME_OPTION_STANDARD);
}


// CefBrowserProcessHandler methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefPrintHandler> App::GetPrintHandler()
{
    return m_linuxPrintHandler;
}

void App::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "App::OnBeforeChildProcessLaunch (CefBrowserProcessHandler)";
    DLOG(INFO) << "|- Process type: " << GetProcessType(command_line);
}

void App::OnContextInitialized()
{
    DLOG(INFO) << "App::OnContextInitialized (CefBrowserProcessHandler)";
    CEF_REQUIRE_UI_THREAD();

    m_initialized = true;
    CefRegisterSchemeHandlerFactory(constants::scheme, "", m_browserHandler);
    m_browserHandler->Process();
}

// CefRenderProcessHandler methods:
// -----------------------------------------------------------------------------
void App::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
{
    DLOG(INFO) << "App::OnBrowserCreated (CefRenderProcessHandler)";
}

void App::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "App::OnBrowserDestroyed (CefRenderProcessHandler)";
}

void App::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "App::OnContextCreated (CefRenderProcessHandler)";
    CEF_REQUIRE_RENDERER_THREAD();
}

void App::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "App::OnContextReleased (CefRenderProcessHandler)";
    CEF_REQUIRE_RENDERER_THREAD();
}

} // namespace cefpdf
