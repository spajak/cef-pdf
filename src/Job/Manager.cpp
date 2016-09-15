#include "Manager.h"
#include "Loader.h"
#include "Printer.h"
#include "ContentProvider.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {
namespace job {

std::size_t Manager::Queue(CefRefPtr<Job> job)
{
    m_jobsQueue.push(job);
    return m_jobsQueue.size();
}

CefRefPtr<CefStreamReader> Manager::GetStreamReader(CefRefPtr<CefBrowser> browser)
{
    auto it = Find(browser);
    DCHECK(it != m_jobs.end());

    CefRefPtr<ContentProvider> provider = new ContentProvider;
    it->job->accept(provider);

    return provider->GetStreamReader();
}

void Manager::Assign(CefRefPtr<CefBrowser> browser)
{
    DCHECK(Find(browser) == m_jobs.end());

    CefRefPtr<Job> job = m_jobsQueue.front();

    DCHECK(job.get());

    m_jobsQueue.pop();

    m_jobs.push_back(BrowserJob({browser, job, ErrorCode::ERR_NONE}));

    // Load URL to print
    CefRefPtr<Loader> loader = new Loader(browser->GetMainFrame());
    job->accept(loader);
}

void Manager::SetError(CefRefPtr<CefBrowser> browser, Manager::ErrorCode errorCode)
{
    auto it = Find(browser);
    DCHECK(it != m_jobs.end());

    it->errorCode = errorCode;
    it->job->OnError("Error loading content");
}

void Manager::Process(CefRefPtr<CefBrowser> browser, int httpStatusCode)
{
    auto it = Find(browser);
    DCHECK(it != m_jobs.end());

    if (it->errorCode != ErrorCode::ERR_NONE) {
        Remove(it);
        return;
    }

    // Print PDF
    CefRefPtr<Printer> printer = new Printer(this, browser);
    it->job->accept(printer);
}

void Manager::Finish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok)
{
    auto it = Find(browser);
    DCHECK(it != m_jobs.end());

    if (!ok) {
        it->job->OnError("Printing PDF failed");
    } else {
        it->job->OnSuccess(path);
    }

    Remove(it);
}

Manager::Iterator Manager::Find(CefRefPtr<CefBrowser> browser)
{
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        if (it->browser->IsSame(browser)) {
            return it;
        }
    }

    return m_jobs.end();
}

void Manager::Remove(Manager::Iterator it)
{
    it->job->OnFinish();
    it->browser->GetHost()->CloseBrowser(true);
    m_jobs.erase(it);
}

} // namespace job
} // namespace cefpdf
