#ifndef CLIENT_H_
#define CLIENT_H_

#include "include/cef_app.h"

#include "Utils.h"
#include "PrintHandler.h"
#include "BrowserHandler.h"

#include <utility>
#include <unordered_map>

#define VERSION "0.1.5"

class Client : public CefApp,
               public CefBrowserProcessHandler
{
    public:

    Client();

    void Initialize();
    void Run();
    void Stop();
    void Shutdown();

    void PostPrintJob(PdfPrintJob printJob);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;
    virtual void OnContextInitialized() OVERRIDE;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    private:

    CefRefPtr<CefPrintHandler> m_printHandler;

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;
    CefString m_defaultEncoding = "utf-8";

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

#endif // CLIENT_H_
