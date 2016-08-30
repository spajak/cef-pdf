#ifndef JOBS_MANAGER_H_
#define JOBS_MANAGER_H_

#include "Job.h"

#include "include/cef_browser.h"

namespace cefpdf {

class JobsManager : public CefBase
{
    public:

    JobsManager() {};

    CefRefPtr<Job> Get(CefRefPtr<CefBrowser> browser);

    CefRefPtr<Job> Find(CefRefPtr<CefBrowser> browser);

    void Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job);

    void OnError(CefRefPtr<CefBrowser> browser, Job::ErrorCode errorCode);

    void OnReady(CefRefPtr<CefBrowser> browser, int httpStatusCode);

    void OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok);

    void Remove(CefRefPtr<CefBrowser> browser);

    private:

    struct BrowserJob {
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<Job> job;
    };

    std::vector<BrowserJob> m_jobs;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(JobsManager);
};

} // namespace cefpdf

#endif // JOBS_MANAGER_H_
