#ifndef JOBS_MANAGER_H_
#define JOBS_MANAGER_H_

#include "Job.h"

#include "include/cef_browser.h"

namespace cefpdf {

class JobsManager : public CefBase
{
    public:

    typedef CefLoadHandler::ErrorCode ErrorCode;

    friend class SchemeHandlerFactory;

    JobsManager() {};

    void Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job);

    void OnError(CefRefPtr<CefBrowser> browser, ErrorCode errorCode);

    void OnReady(CefRefPtr<CefBrowser> browser, int httpStatusCode);

    void Remove(CefRefPtr<CefBrowser> browser);

    void OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok);

    private:

    enum struct Status { Loading, Error, Printing, Done };

    struct JobContainer {
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<Job> job;
        Status status;
        ErrorCode errorCode;
    };

    std::vector<JobContainer> m_jobs;

    typedef std::vector<JobContainer>::iterator JCI;

    JCI GetJobContainer(CefRefPtr<CefBrowser> browser);

    JCI FindJobContainer(CefRefPtr<CefBrowser> browser);

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(JobsManager);
};

} // namespace cefpdf

#endif // JOBS_MANAGER_H_
