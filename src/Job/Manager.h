#ifndef JOB_MANAGER_H_
#define JOB_MANAGER_H_

#include "Job.h"
#include "ContentProvider.h"

#include "include/cef_browser.h"
#include "include/cef_load_handler.h"

#include <vector>
#include <queue>

namespace cefpdf {
namespace job {

class Manager : public CefBase
{

public:
    typedef CefLoadHandler::ErrorCode ErrorCode;

    Manager() {};

    std::size_t Queue(CefRefPtr<Job> job);

    void Assign(CefRefPtr<CefBrowser> browser);

    CefRefPtr<ContentProvider> GetContentProvider(CefRefPtr<CefBrowser> browser);

    void SetError(CefRefPtr<CefBrowser> browser, ErrorCode errorCode);

    void Process(CefRefPtr<CefBrowser> browser, int httpStatusCode);

    void Finish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok);

private:
    std::queue<CefRefPtr<Job>> m_jobsQueue;

    struct BrowserJob {
        CefRefPtr<CefBrowser> browser;
        CefRefPtr<Job> job;
        ErrorCode errorCode;
    };

    std::vector<BrowserJob> m_jobs;

    typedef std::vector<BrowserJob>::iterator Iterator;

    Iterator Find(CefRefPtr<CefBrowser> browser);

    void Remove(Manager::Iterator it);

    unsigned int m_counter = 0;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Manager);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_MANAGER_H_
