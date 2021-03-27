// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "PrintJobBrowser.h"
#include "Common.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

PrintJobBrowser::PrintJobBrowser(CefRefPtr<PrintJob> job, CefRefPtr<CefBrowser> browser) :
    m_job(job),
    m_browser(browser)
{}

CefRefPtr<PrintJob> PrintJobBrowser::GetPrintJob()
{
    return m_job;
}

CefRefPtr<CefBrowser> PrintJobBrowser::GetBrowser()
{
    return m_browser;
}

void PrintJobBrowser::Load()
{
    m_job->SetStatus(PrintJob::Status::LOADING);
    m_browser->GetMainFrame()->LoadURL(m_job->GetUrl());
}

void PrintJobBrowser::Print()
{
    if (m_job->GetOutputPath().empty()) {
        m_job->SetOutputPath(reserveTempFile());
    }
    m_job->SetStatus(PrintJob::Status::PRINTING);
    m_browser->GetHost()->PrintToPDF(
        m_job->GetOutputPath(),
        m_job->GetCefPdfPrintSettings(),
        this
    );
}

void PrintJobBrowser::CloseBrowser()
{
    m_browser->StopLoad();
    m_browser->GetHost()->CloseBrowser(true);
}

void PrintJobBrowser::SetError(PrintJob::ErrorCode code, const CefString& message)
{
    m_job->SetStatus(PrintJob::Status::FAILED);
    m_job->SetError({code, message});
}

// CefPdfPrintCallback methods:
// -----------------------------------------------------------------------------
void PrintJobBrowser::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO) << "PrintJobBrowser::OnPdfPrintFinished (CefPdfPrintCallback)";
    DLOG(INFO) << "|- Path: " << path;
    DLOG(INFO) << "|- Status: " << (ok ? "OK" : "FAILED");

    if (ok) {
        m_job->SetStatus(PrintJob::Status::SUCCESS);
    } else {
        SetError(PrintJob::ErrorCode::PRINT, "Printing to \"" + path.ToString() + "\" failed");
    }

    CloseBrowser();
}

} // namespace cefpdf
