// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_APP_H_
#define CEFPDF_APP_H_

#include "PrintJob.h"
#include "BrowserHandler.h"
#include "LinuxPrintHandler.h"
#include "PrintJobCallback.h"

#include "include/cef_app.h"
#include "include/cef_stream.h"
#include "include/cef_browser_process_handler.h"
#include "include/cef_render_process_handler.h"

namespace cefpdf {

class App : public CefApp,
            public CefBrowserProcessHandler,
            public CefRenderProcessHandler,
            public PrintJobCallback
{

public:
    explicit App(const CefMainArgs&, bool enableJavaScript = false);

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void AddPrintJob(CefRefPtr<PrintJob>);
    void Run();
    void Stop();

    CefRefPtr<CefStreamReader> CreateStreamReaderFromString(const CefString& content);
    CefRefPtr<CefStreamReader> CreateStreamReaderFromStdInput();
    CefRefPtr<CefStreamReader> CreateStreamReaderFromFile(const CefString& fileName);

    CefString GetProcessType(CefRefPtr<CefCommandLine>);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    // CefRenderProcessHandler methods:
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info) override;
    virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;
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

    // JobPrintCallback methods:
    virtual void OnPrintStarted(CefRefPtr<PrintJob>) override;
    virtual void OnPrintFinished(CefRefPtr<PrintJob>) override;

private:
    const CefMainArgs m_mainArgs;
    CefRefPtr<BrowserHandler> m_browserHandler;
    CefRefPtr<LinuxPrintHandler> m_linuxPrintHandler;
    bool m_initialized;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(App);
};

} // namespace cefpdf

#endif // CEFPDF_APP_H_
