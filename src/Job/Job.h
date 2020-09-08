#ifndef JOB_JOB_H_
#define JOB_JOB_H_

#include "../Common.h"
#include "Visitor.h"

#include "include/cef_base.h"

#include <string>
#include <functional>

namespace cefpdf {
namespace job {

class Job : public CefBaseRefCounted
{

public:
    typedef std::function<void(CefRefPtr<Job>)> Callback;

    enum struct Status {
        PENDING,
        LOADING,
        PRINTING,
        SUCCESS,
        HTTP_ERROR,
        ABORTED,
        LOAD_ERROR,
        PRINT_ERROR
    };

    Job();

    void SetCallback(Callback callback) {
        m_callback = callback;
    }

    void ExecuteCallback() {
        if (m_callback != nullptr) {
            m_callback(this);
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

    void SetScale(int scale);

    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings() const;

    Status GetStatus() {
        return m_status;
    }

    void SetStatus(Status status) {
        m_status = status;
    }

private:
    CefString m_outputPath;
    PageSize m_pageSize;
    PageOrientation m_pageOrientation;
    PageMargin m_pageMargin;
    bool m_backgrounds;
    Status m_status;
    Callback m_callback;
    int m_scale;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_JOB_H_
