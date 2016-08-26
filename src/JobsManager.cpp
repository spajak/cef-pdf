#include "BrowserHandler.h"
#include "PdfPrintJob.h"
#include "RenderHandler.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <iostream>

namespace cefpdf {

JobsManager::JobsManager();

void JobsManager::Add(CefRefPtr<CefBrowser> browser, CefRefPtr<Job> job);
{
    if (Exists(browser)) {
        throw "Browser already added";
    }

    JobContainer container;
    container.browser = browser;
    container.job = job;
    m_jobs.push_back(container);

    // Load URL
    browser->GetMainFrame()->LoadURL(job->GetUrl());
}

void JobsManager::OnError(CefRefPtr<CefBrowser> browser, CefLoadHandler::ErrorCode errorCode)
{
    auto j = FindJob(browser);

    j->status = Status::ERROR;
    j->errorCode = errorCode;
}

void JobsManager::OnReady(CefRefPtr<CefBrowser> browser)
{
    auto j = FindJob(browser);

    if (Status::ERROR == j->status) {
        j->browser->GetHost()->CloseBrowser(true);
        return;
    }

    j->status = Status::PRINTING;
    j->browser->GetHost()->PrintToPDF(
        j->job->GetOutputPath(),
        j->job->GetCefPdfPrintSettings(),
        new PdfPrintCallback(this)
    );
}

bool JobsManager::Remove(CefRefPtr<CefBrowser> browser)
{
    auto j = FindJob(browser);

    m_jobs.erase(j);
    return true;
}

void JobsManager::OnFinish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok)
{
    auto j = FindJob(browser);

    j->browser->GetHost()->CloseBrowser(true);
    j->status = ok ? Status::DONE : Status::PRINT_FAILED;
}

bool JobsManager::Exists(CefRefPtr<CefBrowser> browser)
{
    try {
        FindJob(browser);
    } catch (std::string e) {
        return false;
    }

    return true;
}

std::vector<PdfPrintJobContainer>::iterator JobsManager::FindJob(CefRefPtr<CefBrowser> browser)
{
    for (it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        if (it->browser->IsSame(browser)) {
            return it;
        }
    }

    throw "Browser not registered in this manager";
}


// CefSchemeHandlerFactory methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> BrowserHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    auto jobContainer = findJob(browser.get());
    return new ResponseHandler(jobContainer->job->GetContent());
}


JobsManager::PdfPrintCallback::PdfPrintCallback(CefRefPtr<JobsManager> manager, CefRefPtr<CefBrowser> browser)
{

}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void JobsManager::PdfPrintCallback::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO)
        << "OnPdfPrintFinished"
        << ", path: " << path.ToString()
        << ", ok: " << ok;

    DCHECK(browser.get());

    manager->OnFinish(browser, path, ok);
}

} // namespace cefpdf
