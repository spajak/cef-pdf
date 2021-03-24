// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_JOB_LOADER_H_
#define CEFPDF_JOB_LOADER_H_

#include "../Common.h"
#include "Visitor.h"
#include "Local.h"
#include "Remote.h"
#include "StdInput.h"

#include "include/cef_browser.h"

namespace cefpdf {
namespace job {

class Loader : public Visitor
{

public:
    explicit Loader(CefRefPtr<CefBrowser> browser) :
        m_browser(browser) {}

    ~Loader() {
        m_browser = NULL;
    }

    virtual void visit(CefRefPtr<Local> job) override {
        m_browser->GetMainFrame()->LoadURL(cefpdf::constants::scheme + "://local");
    }

    virtual void visit(CefRefPtr<Remote> job) override {
        m_browser->GetMainFrame()->LoadURL(job->GetUrl());
    }

    virtual void visit(CefRefPtr<StdInput> job) override {
        m_browser->GetMainFrame()->LoadURL(cefpdf::constants::scheme + "://stdin");
    }

private:
    CefRefPtr<CefBrowser> m_browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Loader);
};

} // namespace job
} // namespace cefpdf

#endif // CEFPDF_JOB_LOADER_H_
