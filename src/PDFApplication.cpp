#include "PDFApplication.h"
#include "PDFBrowserHandler.h"
#include "StringSchemeHandlerFactory.h"

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

    //CefRefPtr<CefCommandLine> cmdLine = CefCommandLine::GetGlobalCommandLine();
    //std::cout << cmdLine->GetSwitchValue("dir").ToString() << std::endl;

    CefRefPtr<StringSchemeHandlerFactory> factory(new StringSchemeHandlerFactory);

    factory->SetString("<html><head></head><body><h1>Hi ółżź</h1></body></html>");

    CefRegisterSchemeHandlerFactory("template", "", factory.get());

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    CefRefPtr<PDFBrowserHandler> client(new PDFBrowserHandler);

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;

    // Create the first browser window.
    CefBrowserHost::CreateBrowser(windowInfo, client.get(), "template://load", browserSettings, NULL);
}
