#ifndef JOB_JOB_H_
#define JOB_JOB_H_

#include "../Common.h"
#include "Visitor.h"

#include "include/cef_base.h"

#include <string>
#include <future>
#include <functional>

namespace cefpdf {
namespace job {

class Job : public CefBaseRefCounted
{

public:
    typedef std::function<void(const std::string&, CefRefPtr<Job>)> Callback;

    Job();

    void SetCallback(Callback callback) {
        m_callback = callback;
    }

    void ExecuteCallback(const std::string& reason) {
        if (m_callback != nullptr) {
            m_callback(reason, this);
        }
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
    Callback m_callback;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job)
};

} // namespace job
} // namespace cefpdf

#endif // JOB_JOB_H_
