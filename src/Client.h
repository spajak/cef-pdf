#ifndef CLIENT_H_
#define CLIENT_H_

#include "Job/Manager.h"
#include "Storage.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser.h"

#include <queue>

namespace cefpdf {

class Client : public CefApp,
               public CefBrowserProcessHandler,
               public CefClient,
               public CefLifeSpanHandler,
               public CefLoadHandler
{

public:
    Client(bool stopAfterLastJob = false);
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    // Run message loop
    virtual void Run();

    // Stop message loop
    void Stop();

    // Add new job to the queue and process it
    void PostJob(CefRefPtr<job::Job> job);

    // Get Storage object
    CefRefPtr<Storage> GetStorage() {
        return m_storage;
    };

    // Get the number of running job processes
    unsigned int GetProcessCount() {
        return m_processCount;
    };

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) override;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;

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
    void ProcessJobsQueue();
    std::queue<CefRefPtr<job::Job>> m_jobsQueue;
    CefRefPtr<Storage> m_storage;

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;
    CefRefPtr<job::Manager> m_jobsManager;
    unsigned int m_processCount;
    bool m_initialized;
    bool m_stopAfterLastJob;

    CefRefPtr<CefPrintHandler> m_printHandler;
    CefRefPtr<CefRenderHandler> m_renderHandler;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

} // namespace cefpdf

#endif // CLIENT_H_
