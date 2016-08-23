#include "Client.h"
#include "Common.h"
#include "BrowserHandler.h"
#include "RenderHandler.h"
#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {

Client::Client()
{
    m_renderHandler = new RenderHandler;
    m_printHandler = new PrintHandler;

    //m_settings.single_process = true;
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = false;

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
    CefMainArgs mainArgs;
    // Initialize CEF in the main process.
    CefInitialize(mainArgs, m_settings, this, NULL);

    int pp = 0;

    while (true) {
        if (!m_jobsQueue.empty()) {
            auto printJob = m_jobsQueue.front();
            m_jobsQueue.pop();
            // Create the browser window.
            CefRefPtr<BrowserHandler> handler = new BrowserHandler(printJob, m_renderHandler);
            CefBrowserHost::CreateBrowser(m_windowInfo, handler.get(), "", m_browserSettings, NULL);
        }

        if (true == m_shouldStop) {
            m_shouldStop = false;
            break;
        }

        ++pp;

        CefDoMessageLoopWork();
    }

    std::cout << pp;
    CefShutdown();
}

void Client::Stop()
{
    m_shouldStop = true;
}

void Client::PostPrintJob(CefRefPtr<PdfPrintJob> printJob)
{
    m_jobsQueue.push(printJob);
}


// CefApp methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

void Client::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme(constants::scheme, false, true, false);
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
}

} // namespace cefpdf
