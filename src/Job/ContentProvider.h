#ifndef JOB_CONTENT_PROVIDER_H_
#define JOB_CONTENT_PROVIDER_H_

#include "Visitor.h"
#include "Local.h"
#include "Remote.h"
#include "StdInput.h"

namespace cefpdf {
namespace job {

class ContentProvider : public Visitor
{
public:
    ContentProvider() {};

    const CefString& GetContent() const {
        return m_content;
    };

    std::size_t GetContentLength() const {
        return m_content.length();
    };

    virtual void visit(CefRefPtr<Local> job) {
        m_content = job->GetContent();
    };

    virtual void visit(CefRefPtr<Remote> job) {
        // no content
    };

    virtual void visit(CefRefPtr<StdInput> job) {
        m_content = job->GetContent();
    };

private:
    CefString m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ContentProvider);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_CONTENT_PROVIDER_H_