#ifndef CLIENT_H_
#define CLIENT_H_

#include "Job/Manager.h"
#include "RequestHandler.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"

#include <queue>
#include <set>

namespace cefpdf {

class Client : public CefApp,
               public CefBrowserProcessHandler,
               public CefClient,
               public CefLifeSpanHandler,
               public CefLoadHandler
{

public:
    Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Execute subprocess, if any
    int ExecuteSubProcess(const CefMainArgs&);

    // Initialize CEF
    void Initialize(const CefMainArgs&);

    // De-initialize CEF
    void Shutdown();

    // Run message loop
    void Run();

    // Stop message loop and/or shutdown CEF
    void Stop();

    // Post delayed stop
    void PostStop();

    // Add new job to the queue and process it
    void AddJob(CefRefPtr<job::Job> job);

    // Add new job to the queue and process it
    void PostJob(CefRefPtr<job::Job> job);

    // Get the number of running job processes
    unsigned int GetProcessCount() {
        return m_browsersCount;
    }

    void SetStopAfterLastJob(bool flag) {
        m_stopAfterLastJob = flag;
    }

    void SetDisableJavaScript(bool flag) {
        m_browserSettings.javascript = flag ? STATE_DISABLED : STATE_ENABLED;
    }

    void SetAllowedSchemes(const std::set<std::string>& schemes) {
        for (auto s: schemes) {
            m_requestHandler->AddAllowedScheme(s);
        }
    }

    void ClearAllowedSchemes() {
        m_requestHandler->ClearAllowedSchemes();
    }

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods:
    virtual void OnLoadStart(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        TransitionType transition_type
    ) override;
    virtual void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode
    ) override;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) override;

private:
    void CreateBrowser();

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;
    CefRefPtr<job::Manager> m_jobsManager;
    unsigned int m_pendingBrowsersCount;
    unsigned int m_browsersCount;
    bool m_initialized;
    bool m_contextInitialized;
    bool m_running;
    bool m_stopAfterLastJob;

    CefRefPtr<CefPrintHandler> m_printHandler;
    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<RequestHandler> m_requestHandler;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client)
};

} // namespace cefpdf

#endif // CLIENT_H_
