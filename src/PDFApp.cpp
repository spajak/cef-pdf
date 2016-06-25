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

    std::cout << "OnContextInitialized" << std::endl;

    std::string url = "https://pl.wikipedia.org/wiki/Referendum_w_Wielkiej_Brytanii_w_2016_roku";

    // Information used when creating the native window.
    CefWindowInfo windowInfo;

    windowInfo.SetAsWindowless(NULL, true);

    CefRefPtr<PDFHandler> client(new PDFHandler);

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client, url, browserSettings, NULL);
}
