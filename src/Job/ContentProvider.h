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

    const std::string& GetContent() const {
        return m_content;
    };

    std::size_t GetContentLength() const {
        return m_content.length();
    };

    const char* GetContentPtr() const {
        return m_content.c_str();
    };

    virtual void visit(CefRefPtr<Local> job) {
        m_content = job->GetContent().ToString();
    };

    virtual void visit(CefRefPtr<Remote> job) {
        // no content
    };

    virtual void visit(CefRefPtr<StdInput> job) {
        m_content = job->GetContent();
    };

private:
    std::string m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ContentProvider);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_CONTENT_PROVIDER_H_