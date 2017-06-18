#ifndef JOB_JOB_H_
#define JOB_JOB_H_

#include "../Common.h"
#include "Visitor.h"

#include "include/cef_base.h"

#include <string>
#include <future>

namespace cefpdf {
namespace job {

class Job : public CefBaseRefCounted
{

public:
    Job();

    std::future<std::string> GetFuture() {
        return m_promise.get_future();
    }

    void Resolve(std::string value) {
        m_promise.set_value(value);
    }

    virtual void accept(CefRefPtr<Visitor> visitor) = 0;

    const CefString& GetOutputPath() const {
        return m_outputPath;
    }

    void SetOutputPath(const CefString& outputPath) {
        m_outputPath = outputPath;
    }

    void SetPageSize(const CefString& pageSize);

    void SetLandscape(bool flag = true);

    void SetPageMargin(const CefString& pageMargin);

    void SetBackgrounds(bool flag = true);

    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings() const;

private:
    CefString m_outputPath;
    PageSize m_pageSize;
    PageOrientation m_pageOrientation;
    PageMargin m_pageMargin;
    bool m_backgrounds;
    std::promise<std::string> m_promise;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job)
};

} // namespace job
} // namespace cefpdf

#endif // JOB_JOB_H_
