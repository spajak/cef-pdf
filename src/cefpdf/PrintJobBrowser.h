// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_PRINT_JOB_BROWSER_H_
#define CEFPDF_PRINT_JOB_BROWSER_H_

#include "PrintJob.h"

#include "include/cef_browser.h"

namespace cefpdf {

class PrintJobBrowser : public CefPdfPrintCallback
{

public:
    PrintJobBrowser(CefRefPtr<PrintJob>, CefRefPtr<CefBrowser>);

    CefRefPtr<PrintJob> GetPrintJob();
    CefRefPtr<CefBrowser> GetBrowser();
    void Load();
    void Print();
    void CloseBrowser();
    void SetError(PrintJob::ErrorCode, const CefString&);

    // CefPdfPrintCallback methods:
    virtual void OnPdfPrintFinished(const CefString& path, bool ok) override;

private:
    CefRefPtr<PrintJob> m_job;
    CefRefPtr<CefBrowser> m_browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PrintJobBrowser);
};

} // namespace cefpdf

#endif // CEFPDF_PRINT_JOB_BROWSER_H_
