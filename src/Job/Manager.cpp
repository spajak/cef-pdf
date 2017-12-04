#include "Manager.h"
#include "Loader.h"
#include "Printer.h"
#include "ContentProvider.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/cef_render_handler.h"

#include <math.h>

namespace cefpdf {
namespace job {

std::size_t Manager::Queue(CefRefPtr<Job> job)
{
    DLOG(INFO) << "Manager::Queue";

    m_jobsQueue.push(job);
    return m_jobsQueue.size();
}

CefRefPtr<CefStreamReader> Manager::GetStreamReader(CefRefPtr<CefBrowser> browser)
{
    auto it = Find(browser);
    DCHECK(it != m_jobs.end());

    if (!it->streamReader.get()) {
        CefRefPtr<ContentProvider> provider = new ContentProvider;
        it->job->accept(provider);
        it->streamReader = provider->GetStreamReader();
    }

    return it->streamReader;
}

void Manager::Assign(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "Manager::Assign";

    DCHECK(Find(browser) == m_jobs.end());

    if (!m_jobsQueue.empty()) {
        CefRefPtr<Job> job = m_jobsQueue.front();

        m_jobsQueue.pop();

        m_jobs.push_back(BrowserJob({browser, job, NULL}));

        job->SetStatus(Job::Status::LOADING);

        // Load URL to print
        CefRefPtr<Loader> loader = new Loader(browser->GetMainFrame());
        job->accept(loader);
    }
}

void Manager::Process(CefRefPtr<CefBrowser> browser, int httpStatusCode)
{
    DLOG(INFO) << "Manager::Process";

    auto it = Find(browser);

    if (it != m_jobs.end()) {
        if (!httpStatusCode || (200 <= httpStatusCode && 300 > httpStatusCode)) {
            // Generate file name if empty
            if (it->job->GetOutputPath().empty()) {
                it->job->SetOutputPath(reserveTempFile());
            }

            it->job->SetStatus(Job::Status::PRINTING);

            // Print PDF
            CefRefPtr<Printer> printer = new Printer(this, browser);
            it->job->accept(printer);
        } else {
            Resolve(it, Job::Status::HTTP_ERROR);
        }
    }
}

void Manager::Finish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok)
{
    auto it = Find(browser);
    if (it != m_jobs.end()) {
        Resolve(it, ok ? Job::Status::SUCCESS : Job::Status::PRINT_ERROR);
    }
}

void Manager::Abort(CefRefPtr<CefBrowser> browser, CefLoadHandler::ErrorCode errorCode)
{
    auto it = Find(browser);
    if (it != m_jobs.end()) {
        Resolve(it, errorCode == CefLoadHandler::ErrorCode::ERR_ABORTED ? Job::Status::ABORTED : Job::Status::LOAD_ERROR);
    }
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

void Manager::Resolve(Manager::Iterator it, const Job::Status& status)
{
    DLOG(INFO) << "Manager::Resolve";

    it->job->SetStatus(status);
    it->job->ExecuteCallback();

    it->browser->GetHost()->CloseBrowser(true);
    m_jobs.erase(it);
}

void Manager::StopAll()
{
    while (!m_jobsQueue.empty()) {
        m_jobsQueue.pop();
    }

    for (auto it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        it->job->SetStatus(Job::Status::ABORTED);
        it->browser->GetHost()->CloseBrowser(true);
    }

    m_jobs.clear();
}

// CefRenderHandler methods:
// -------------------------------------------------------------------------
bool Manager::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    auto it = Find(browser);
    if (it != m_jobs.end()) {
        CefPdfPrintSettings settings = it->job->GetCefPdfPrintSettings();

        int horizontalMargin;
        int verticalMargin;

        if (settings.margin_type == PDF_PRINT_MARGIN_DEFAULT) {
           if (settings.page_width >= 25400) {
              horizontalMargin = 20000;
           } else {
               horizontalMargin = 0;
           }
           if (settings.page_height >= 25400) {
               verticalMargin = 20000;
           } else {
               verticalMargin = 0;
           }
        } else if (settings.margin_type == PDF_PRINT_MARGIN_MINIMUM) {
           horizontalMargin = 0;
           verticalMargin = 0;
        } else if (settings.margin_type == PDF_PRINT_MARGIN_NONE) {
           horizontalMargin = 0;
           verticalMargin = 0;
        } else { // Custom
           horizontalMargin = ceil((settings.margin_left + settings.margin_right) * 1000);
           verticalMargin = ceil((settings.margin_top + settings.margin_bottom) * 1000);
        }

        int width = floor((settings.page_width - horizontalMargin) * (1 / constants::micronPerPx));
        int height = floor((settings.page_height - verticalMargin) * (1 / constants::micronPerPx));

        rect.x = 0;
        rect.y = 0;

        if (settings.landscape) {
          rect.width = height;
          rect.height = width;
        } else {
          rect.width = width;
          rect.height = height;
        }

        return true;
    }

    return false;
}

void Manager::OnPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    const void* buffer, int width, int height
) {

}

} // namespace job
} // namespace cefpdf
