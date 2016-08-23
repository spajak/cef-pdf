#ifndef CLIENT_H_
#define CLIENT_H_

#include "include/cef_app.h"

#include "PrintHandler.h"
#include "BrowserHandler.h"

#include <utility>
#include <unordered_map>
#include <queue>

namespace cefpdf {

class Client : public CefApp,
               public CefBrowserProcessHandler
{
    public:

    typedef std::queue<CefRefPtr<PdfPrintJob>> JobsQueue;

    Client();

    // Run message loop
    void Run();

    // Stop message loop
    void Stop();

    void PostPrintJob(CefRefPtr<PdfPrintJob> printJob);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;
    virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;
    virtual void OnContextInitialized() OVERRIDE;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    private:

    JobsQueue m_jobsQueue;

    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<CefPrintHandler> m_printHandler;

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;

    bool m_shouldStop = false;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

} // namespace cefpdf

#endif // CLIENT_H_
