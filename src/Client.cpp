#include "Client.h"
#include "StdInputSchemeHandlerFactory.h"
#include "BrowserHandler.h"
#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

Client::Client()
{
    m_printHandler = new PrintHandler;

    //m_settings.single_process = true;
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = false;

    m_windowInfo.windowless_rendering_enabled = true;
    m_windowInfo.transparent_painting_enabled = false;

    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(m_defaultEncoding);
    m_browserSettings.javascript_open_windows = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
    m_browserSettings.plugins = STATE_DISABLED;
}

void Client::Initialize()
{
    CefMainArgs mainArgs;
    // Initialize CEF in the main process.
    CefInitialize(mainArgs, m_settings, this, NULL);
}

void Client::Run()
{
    Initialize();
    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();
    Shutdown();
}

void Client::Stop()
{
    // Stop message loop
    CefQuitMessageLoop();
}

void Client::Shutdown()
{
    // Shut down CEF.
    CefShutdown();
}

void Client::PostPrintJob(PdfPrintJob printJob)
{
    // Create the browser window.
    CefRefPtr<CefBrowser> handler = new BrowserHandler(printJob);
    CefBrowserHost::CreateBrowser(m_windowInfo, handler.get(), "about:blank", m_browserSettings, NULL);
}


// CefApp methods:
// -------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

// CefBrowserProcessHandler methods:
// -------------------------------------------------------------------------
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

    // Register scheme handler factory for "stdin" scheme. Allows response for url
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());
}
