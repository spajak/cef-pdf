#ifndef JOB_LOCAL_H_
#define JOB_LOCAL_H_

#include "Job.h"

namespace cefpdf {
namespace job {

class Local : public Job
{

public:
    Local(const std::string& content) : Job(), m_content(content) {}

    virtual const std::string& GetContent() const {
        return m_content;
    }

    void SetContent(const std::string& content) {
        m_content = content;
    }

    virtual void accept(CefRefPtr<Visitor> visitor) override {
        visitor->visit(this);
    }

private:
    std::string m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Local);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_LOCAL_H_
