#include "PDFApp.h"
#include "PDFHandler.h"
#include <string>
#include <iostream>
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"

PDFApp::PDFApp() {}

CefRefPtr<CefBrowserProcessHandler> PDFApp::GetBrowserProcessHandler()
{
    return this;
}

void PDFApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    std::string url = "template://load";

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    CefRefPtr<PDFHandler> client(new PDFHandler);

    CefRegisterSchemeHandlerFactory("template", "", client.get());

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client.get(), url, browserSettings, NULL);
}
