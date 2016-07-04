#include "Application.h"
#include "BrowserHandler.h"
#include "StdInputSchemeHandlerFactory.h"
#include "PrintHandler.h"

#include <string>
#include <iostream>
#include "include/wrapper/cef_helpers.h"


Application::Application() {}

Application::Application(CefRefPtr<CefCommandLine> commandLine)
{
    m_commandLine = commandLine;
}

CefRefPtr<CefBrowserProcessHandler> Application::GetBrowserProcessHandler()
{
    return this;
}

void Application::OnContextInitialized()
{
    DLOG(INFO) << "Application::OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    // Register scheme handler factory for "stdin" scheme. Allows response for url like
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    //BrowserHandler::Method method;
    CefString url;

    if (m_commandLine->HasSwitch("url")) {
        url = m_commandLine->GetSwitchValue("url");
    } else {
        url = "stdin://get";
    }

    CefRefPtr<BrowserHandler> client(new BrowserHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client.get(), url, browserSettings, NULL);
}

CefRefPtr<CefPrintHandler> Application::GetPrintHandler()
{
    return new PrintHandler;
}
