#include "JobsManager.h"
#include "RenderHandler.h"
#include "PdfPrintCallback.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

CefRefPtr<Job> JobsManager::Get(CefRefPtr<CefBrowser> browser)
{
    auto job = Find(browser);
    DCHECK(job.get());
    return job;
}

CefRefPtr<Job> JobsManager::Find(CefRefPtr<CefBrowser> browser)
{
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        if (it->browser->IsSame(browser)) {
            return it->job;
        }
    }

    return NULL;
}

void JobsManager::Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job)
{
    DCHECK(!Find(browser).get());

    m_jobs.push_back(BrowserJob({browser, job}));

    job->ChangeStatus(Job::Status::Loading);

    // Load URL
    browser->GetMainFrame()->LoadURL(job->GetUrl());
}

void JobsManager::OnError(CefRefPtr<CefBrowser> browser, Job::ErrorCode errorCode)
{
    auto job = Get(browser);
    DCHECK(Job::Status::Loading == job->GetStatus());

    job->ChangeStatus(Job::Status::LoadError, errorCode);
}

void JobsManager::OnReady(CefRefPtr<CefBrowser> browser, int httpStatusCode)
{
    auto job = Get(browser);

    if (Job::Status::LoadError == job->GetStatus()) {
        browser->GetHost()->CloseBrowser(true);
        return;
    }

    DCHECK(Job::Status::Loading == job->GetStatus());

    job->ChangeStatus(Job::Status::Printing);

    browser->GetHost()->PrintToPDF(
        job->GetOutputPath(),
        job->GetCefPdfPrintSettings(),
        new PdfPrintCallback(this, browser)
    );
}

void JobsManager::OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok)
{
    auto job = Get(browser);
    DCHECK(Job::Status::Printing == job->GetStatus());

    job->ChangeStatus(ok ? Job::Status::Success : Job::Status::Failed);
    browser->GetHost()->CloseBrowser(true);
}

void JobsManager::Remove(CefRefPtr<CefBrowser> browser)
{
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        if (it->browser->IsSame(browser)) {
            DCHECK(Job::Status::Loading != it->job->GetStatus());
            DCHECK(Job::Status::Printing != it->job->GetStatus());
            m_jobs.erase(it);
            break;
        }
    }
}

} // namespace cefpdf
