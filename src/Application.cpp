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
    DLOG(INFO) << "OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    // Register scheme handler factory for "stdin" scheme. Allows response for url like
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());

    // Set default string encoding to UTF-8
    CefRefPtr<CefValue> value = CefValue::Create();
    value->SetString("utf-8");
    CefString error;
    CefRequestContext::GetGlobalContext()
        ->SetPreference("intl.charset_default", value, error);

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    // PDF parameters
    BrowserHandler::PDFParameters parameters;

    if (m_commandLine->HasSwitch("url")) {
        parameters.url = m_commandLine->GetSwitchValue("url");
    }

    if (m_commandLine->HasSwitch("output")) {
        parameters.output = m_commandLine->GetSwitchValue("output");
    }

    if (m_commandLine->HasSwitch("paper-size")) {
        parameters.paperSize = m_commandLine->GetSwitchValue("paper-size");
    }

    if (m_commandLine->HasSwitch("landscape")) {
        parameters.landscape = true;
    }

    CefRefPtr<BrowserHandler> client(new BrowserHandler(parameters));

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client.get(), "about:blank", browserSettings, NULL);
}

CefRefPtr<CefPrintHandler> Application::GetPrintHandler()
{
    return new PrintHandler;
}
