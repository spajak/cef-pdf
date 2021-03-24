// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_JOB_REMOTE_H_
#define CEFPDF_JOB_REMOTE_H_

#include "Job.h"

namespace cefpdf {
namespace job {

class Remote : public Job
{

public:
    explicit Remote(const CefString& url) :
        Job(), m_url(url) {}

    const CefString& GetUrl() const {
        return m_url;
    }

    virtual void accept(CefRefPtr<Visitor> visitor) override {
        visitor->visit(this);
    }

private:
    CefString m_url;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Remote);
};

} // namespace job
} // namespace cefpdf

#endif // CEFPDF_JOB_REMOTE_H_
