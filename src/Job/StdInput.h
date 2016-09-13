#ifndef JOB_STDINPUT_H_
#define JOB_STDINPUT_H_

#include "Job.h"

namespace cefpdf {
namespace job {

class StdInput : public Job
{

public:
    const std::string& GetContent();

    virtual void accept(CefRefPtr<Visitor> visitor) {
        visitor->visit(this);
    };

private:
    std::string m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInput);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_STDINPUT_H_