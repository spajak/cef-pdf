// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_APP_H_
#define CEFPDF_APP_H_

#include <queue>

#include "Client.h"
#include "ClientHandler.h"
#include "PrintHandler.h"
#include "SchemeHandlerFactory.h"

#include "include/cef_app.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_render_process_handler.h"

namespace cefpdf {

class App : public CefApp,
            public CefBrowserProcessHandler,
            public CefRenderProcessHandler,
            public ClientHandler
{

public:
    typedef std::queue<CefRefPtr<Client>> ClientQueue;

    explicit App(const CefMainArgs&);

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void Queue(CefRefPtr<job::Job>);
    void Run();
    void Stop();

    CefString GetProcessType(CefRefPtr<CefCommandLine>);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    // CefRenderProcessHandler methods:
    virtual void OnContextCreated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context
    ) override;

    virtual void OnContextReleased(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context
    ) override;

    // ClientHandler methods:
    void OnFinished(CefRefPtr<job::Job>, Client::Status) override;

private:
    const CefMainArgs m_mainArgs;
    CefRefPtr<CefPrintHandler> m_printHandler;
    CefRefPtr<SchemeHandlerFactory> m_schemeHandlerFactory;
    bool m_initialized;
    ClientQueue m_queue;

    void ProcessQueue();

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(App);
};

} // namespace cefpdf

#endif // CEFPDF_APP_H_
