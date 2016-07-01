#include "PDFApplication.h"
#include "PDFBrowserHandler.h"
#include "StdInputSchemeHandlerFactory.h"

#include <string>
#include <iostream>
#include "include/wrapper/cef_helpers.h"


PDFApplication::PDFApplication() {}

PDFApplication::PDFApplication(CefRefPtr<CefCommandLine> commandLine)
{
    m_commandLine = commandLine;
}

CefRefPtr<CefBrowserProcessHandler> PDFApplication::GetBrowserProcessHandler()
{
    return this;
}

void PDFApplication::OnContextInitialized()
{
    DLOG(INFO) << "PDFApplication::OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    // Register scheme handler factory for "stdin" scheme. Allows response for url like
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    //PDFBrowserHandler::Method method;
    CefString url;

    if (m_commandLine->HasSwitch("url")) {
        url = m_commandLine->GetSwitchValue("url");
    } else {
        url = "stdin://get";
    }

    CefRefPtr<PDFBrowserHandler> client(new PDFBrowserHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client.get(), url, browserSettings, NULL);
}
