#ifndef JOB_STDINPUT_H_
#define JOB_STDINPUT_H_

#include "Job.h"

namespace cefpdf {
namespace job {

class StdInput : public Job
{

public:
    virtual void accept(CefRefPtr<Visitor> visitor) {
        visitor->visit(this);
    };

private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInput);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_STDINPUT_H_