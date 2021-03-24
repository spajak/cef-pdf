// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_JOB_PRINTER_H_
#define CEFPDF_JOB_PRINTER_H_

#include "../Client.h"
#include "Visitor.h"
#include "Local.h"
#include "Remote.h"
#include "StdInput.h"

#include "include/cef_browser.h"

namespace cefpdf {
namespace job {

class Printer : public Visitor
{

public:
    Printer(CefRefPtr<CefPdfPrintCallback> client, CefRefPtr<CefBrowser> browser) :
        m_client(client),
        m_browser(browser) {}

    void Print(CefRefPtr<Job> job) {
        m_browser->GetHost()->PrintToPDF(
            job->GetOutputPath(),
            job->GetCefPdfPrintSettings(),
            m_client
        );
    }

    virtual void visit(CefRefPtr<Local> job) override {
        Print(job);
    }

    virtual void visit(CefRefPtr<Remote> job) override {
        Print(job);
    }

    virtual void visit(CefRefPtr<StdInput> job) override {
        Print(job);
    }

private:
    CefRefPtr<CefPdfPrintCallback> m_client;
    CefRefPtr<CefBrowser> m_browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Printer);
};

} // namespace job
} // namespace cefpdf

#endif // CEFPDF_JOB_PRINTER_H_
