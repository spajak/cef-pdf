#include "JobsManager.h"
#include "RenderHandler.h"
#include "PdfPrintCallback.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

void JobsManager::Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job)
{
    DCHECK(FindJobContainer(browser) == m_jobs.end());

    auto j = JobContainer({browser, job, Status::Loading, ErrorCode::ERR_NONE});
    m_jobs.push_back(j);

    // Load URL
    browser->GetMainFrame()->LoadURL(job->GetUrl());
}

void JobsManager::OnError(CefRefPtr<CefBrowser> browser, ErrorCode errorCode)
{
    auto j = GetJobContainer(browser);
    DCHECK(Status::Loading == j->status);

    j->status = Status::Error;
    j->errorCode = errorCode;
}

void JobsManager::OnReady(CefRefPtr<CefBrowser> browser, int httpStatusCode)
{
    auto j = GetJobContainer(browser);

    if (Status::Error == j->status) {
        j->browser->GetHost()->CloseBrowser(true);
        return;
    }

    DCHECK(Status::Loading == j->status);

    j->status = Status::Printing;
    j->browser->GetHost()->PrintToPDF(
        j->job->GetOutputPath(),
        j->job->GetCefPdfPrintSettings(),
        new PdfPrintCallback(this, j->browser)
    );
}

void JobsManager::Remove(CefRefPtr<CefBrowser> browser)
{
    auto j = GetJobContainer(browser);
    DCHECK(Status::Loading != j->status);
    DCHECK(Status::Printing != j->status);
    m_jobs.erase(j);
}

void JobsManager::OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok)
{
    auto j = GetJobContainer(browser);
    DCHECK(Status::Printing == j->status);

    j->status = Status::Done;
    j->browser->GetHost()->CloseBrowser(true);
}

JobsManager::JCI JobsManager::GetJobContainer(CefRefPtr<CefBrowser> browser)
{
    auto j = FindJobContainer(browser);
    DCHECK(j != m_jobs.end());
    return j;
}

JobsManager::JCI JobsManager::FindJobContainer(CefRefPtr<CefBrowser> browser)
{
    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        if (it->browser->IsSame(browser)) {
            return it;
        }
    }

    return m_jobs.end();
}

} // namespace cefpdf
