#include "Printer.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {
namespace job {

Printer::Printer(CefRefPtr<Manager> manager, CefRefPtr<CefBrowser> browser) :
    m_manager(manager),
    m_browser(browser)
{}

void Printer::Print(CefRefPtr<Job> job)
{
    auto path = job->GetOutputPath();
    DLOG(INFO) << "Printer::Print";
    if (path.empty()) {

    }

    m_browser->GetHost()->PrintToPDF(path, job->GetCefPdfPrintSettings(), this);
}

void Printer::visit(CefRefPtr<Local> job)
{
    Print(job.get());
}

void Printer::visit(CefRefPtr<Remote> job)
{
    Print(job.get());
}

void Printer::visit(CefRefPtr<StdInput> job)
{
    Print(job.get());
}

// CefPdfPrintCallback methods:
void Printer::OnPdfPrintFinished(const CefString& path, bool ok)
{
    m_manager->Finish(m_browser, path, ok);
}

} // namespace job
} // namespace cefpdf
