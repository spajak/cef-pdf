// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_JOB_CONTENT_PROVIDER_H_
#define CEFPDF_JOB_CONTENT_PROVIDER_H_

#include "Visitor.h"
#include "Local.h"
#include "Remote.h"
#include "StdInput.h"
#include "../StdInputStreamReader.h"

namespace cefpdf {
namespace job {

class ContentProvider : public Visitor
{

public:
    ContentProvider() : m_reader(NULL) {}

    CefRefPtr<CefStreamReader> GetStreamReader() const {
        return m_reader;
    }

    virtual void visit(CefRefPtr<Local> job) override {
        auto content = job->GetContent();
        if (content.empty()) {
            content = "<!DOCTYPE html>";
        }

        m_reader = CefStreamReader::CreateForData(
            static_cast<void*>(const_cast<char*>(content.c_str())),
            content.size()
        );
    }

    virtual void visit(CefRefPtr<Remote> job) override {
        // no implementation
    }

    virtual void visit(CefRefPtr<StdInput> job) override {
        m_reader = CefStreamReader::CreateForHandler(new StdInputStreamReader);
    }

private:
    CefRefPtr<CefStreamReader> m_reader;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ContentProvider);
};

} // namespace job
} // namespace cefpdf

#endif // CEFPDF_JOB_CONTENT_PROVIDER_H_
