#include "PDFApp.h"
#include <string>
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

    std::string url = "http://www.google.com";

    // Information used when creating the native window.
    CefWindowInfo windowInfo;

    windowInfo.SetAsWindowless(NULL, true);

    CefRefPtr<CefClient> client;

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client, url, browserSettings, NULL);
}
