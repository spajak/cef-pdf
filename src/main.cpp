#include "PDFApp.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    std::string url = "http://www.google.com";

    // Information used when creating the native window.
    CefWindowInfo windowInfo;

    windowInfo.SetAsWindowless(NULL, true);

    CefRefPtr<CefClient> client;

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client, url, browserSettings, NULL);

    return 0;
}
