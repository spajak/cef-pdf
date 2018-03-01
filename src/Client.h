#ifndef CLIENT_H_
#define CLIENT_H_

#include "Job/Manager.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_request_handler.h"
#include "include/wrapper/cef_message_router.h"

#include <queue>
#include <set>

namespace cefpdf {

class Client : public CefApp,
               public CefBrowserProcessHandler,
               public CefClient,
               public CefLifeSpanHandler,
               public CefLoadHandler,
               public CefRequestHandler,
               public CefMessageRouterBrowserSide::Handler
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

    // Add new job to the queue and process it
    void AddJob(CefRefPtr<job::Job> job);

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

    void AddAllowedScheme(const std::string& scheme) {
        m_schemes.insert(scheme);
    }

    void SetAllowedSchemes(const std::set<std::string>& schemes) {
        for (const auto &s: schemes) {
            AddAllowedScheme(s);
        }
    }

    void ClearAllowedSchemes() {
        m_schemes.clear();
    }

    void RemoveAllowedScheme(const std::string& scheme) {
        auto i = m_schemes.find(scheme);
        if (i != m_schemes.end()) {
            m_schemes.erase(i);
        }
    }

    void SetRemoteTrigger(bool flag = true);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;
    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message
    ) override;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods:
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

    // CefRequestHandler methods:
    virtual bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_redirect
    ) override;

    virtual void OnRenderProcessTerminated(
        CefRefPtr<CefBrowser> browser,
        CefRequestHandler::TerminationStatus status
    ) override;

    // CefMessageRouterBrowserSide::Handler methods:
    virtual bool OnQuery(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int64 query_id,
        const CefString& request,
        bool persistent,
        CefRefPtr<Callback> callback
    ) override;

private:
    void CreateBrowsers(unsigned int browserCount = 0);

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;
    CefRefPtr<job::Manager> m_jobManager;
    std::set<std::string> m_schemes;
    unsigned int m_pendingBrowsersCount;
    unsigned int m_browsersCount;
    bool m_initialized;
    bool m_contextInitialized;
    bool m_running;
    bool m_stopAfterLastJob;
    bool m_remoteTrigger;

    CefRefPtr<CefPrintHandler> m_printHandler;
    CefRefPtr<CefRenderHandler> m_renderHandler;
    CefRefPtr<CefRenderProcessHandler> m_renderProcessHandler;
    CefRefPtr<CefMessageRouterBrowserSide> m_messageRouterBrowserSide;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client)
};

} // namespace cefpdf

#endif // CLIENT_H_
