#ifndef JOB_LOCAL_H_
#define JOB_LOCAL_H_

#include "Job.h"

namespace cefpdf {
namespace job {

class Local : public Job
{

public:
    virtual const CefString& GetContent() const {
        return m_content;
    };

    void SetContent(const CefString& content) {
        m_content = content;
    };

    virtual void accept(CefRefPtr<Visitor> visitor) {
        visitor->visit(this);
    };

private:
    CefString m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Local);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_LOCAL_H_