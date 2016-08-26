#ifndef JOBS_MANAGER_H_
#define JOBS_MANAGER_H_

#include "Job.h"

#include "include/cef_browser.h"
#include "include/cef_scheme.h"

namespace cefpdf {

class JobsManager : public CefSchemeHandlerFactory
{
    public:

    JobsManager();

    void Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job);

    void OnError(CefRefPtr<CefBrowser> browser, CefLoadHandler::ErrorCode errorCode);

    void OnReady(CefRefPtr<CefBrowser> browser);

    CefRefPtr<Job> Remove(CefRefPtr<CefBrowser> browser);


    void OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok);

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERRIDE;

    private:

    enum struct Status { LOADING = 0, ERROR, PRINTING, PRINT_FAILED, DONE };

    struct JobContainer {
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<Job> job;
        Status status = Status::LOADING;
        CefLoadHandler::ErrorCode errorCode = ERR_NONE;
    };

    class PdfPrintCallback {
        public:
        PdfPrintCallback(CefRefPtr<JobsManager> manager, CefRefPtr<CefBrowser> browser);
        void OnPdfPrintFinished(const CefString& path, bool ok) OVERRIDE;
        private:
        CefRefPtr<JobsManager> m_manager;
        CefRefPtr<CefBrowser> m_browser;
    };

    std::vector<JobContainer> m_jobs;

    std::vector<JobContainer>::iterator FindJob(CefRefPtr<CefBrowser> browser);

    bool Exists(CefRefPtr<CefBrowser> browser);

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(JobsManager);
};

} // namespace cefpdf

#endif // BROWSER_HANDLER_H_
