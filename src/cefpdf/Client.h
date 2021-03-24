// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_CLIENT_H_
#define CEFPDF_CLIENT_H_

#include <set>
#include <string>

#include "RenderHandler.h"
#include "SchemeHandlerFactory.h"
#include "job/Job.h"

#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_stream.h"

namespace cefpdf {

class ClientHandler;

class Client : public CefClient,
               public CefLifeSpanHandler,
               public CefLoadHandler,
               public CefRequestHandler,
               public CefPdfPrintCallback
{

public:
    enum struct Status {
        PENDING,
        LOADING,
        PRINTING,
        SUCCESS,
        HTTP_ERROR,
        ABORTED,
        NET_ERROR,
        PRINT_ERROR
    };

    Client(CefRefPtr<job::Job>, CefRefPtr<ClientHandler>, CefRefPtr<SchemeHandlerFactory>);

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void Start();

    Status GetStatus();

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
/*
    void SetViewWidth(int viewWidth);
    void SetViewHeight(int viewHeight);

    void Client::SetViewWidth(int viewWidth)
    {
       m_renderHandler->SetViewWidth(viewWidth);
    }

    void Client::SetViewHeight(int viewHeight)
    {
       m_renderHandler->SetViewHeight(viewHeight);
    }

    void Process(CefRefPtr<CefBrowser> browser);
*/
    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;
    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message
    ) override;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods:
    virtual void OnLoadStart(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefLoadHandler::TransitionType transition_type
    ) override;
    virtual void OnLoadEnd(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode
    ) override;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefLoadHandler::ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl
    ) override;

    // CefRequestHandler methods:
    virtual bool OnBeforeBrowse(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool user_gesture,
        bool is_redirect
    ) override;

    // CefPdfPrintCallback methods:
    virtual void OnPdfPrintFinished(const CefString& path, bool ok) override;

private:
    CefRefPtr<job::Job> m_job;
    CefRefPtr<ClientHandler> m_handler;
    CefRefPtr<RenderHandler> m_renderHandler;
    CefRefPtr<SchemeHandlerFactory> m_schemeHandlerFactory;
    Status m_status;
    CefRefPtr<CefBrowser> m_browser;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;
    std::set<std::string> m_schemes;

    void Stop();
    void Print();

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

} // namespace cefpdf

#endif // CEFPDF_CLIENT_H_
