#ifndef JOB_JOB_H_
#define JOB_JOB_H_

#include "../Common.h"
#include "Visitor.h"

#include "include/cef_base.h"

namespace cefpdf {
namespace job {

class Job : public CefBase
{

public:
    Job();

    virtual void OnSuccess(const CefString& path) {};

    virtual void OnError(const CefString& message) {};

    virtual void OnFinish() {};


    virtual void accept(CefRefPtr<Visitor> visitor) = 0;


    const CefString& GetOutputPath() const {
        return m_outputPath;
    };

    void SetOutputPath(const CefString& outputPath) {
        m_outputPath = outputPath;
    };

    void SetPageSize(const CefString& pageSize);

    void SetLandscape(bool flag = true);

    void SetPageMargin(const CefString& pageMargin);

    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings() const;

private:
    CefString m_outputPath;

    PageSize m_pageSize;
    PageOrientation m_pageOrientation = PageOrientation::PORTRAIT;
    PageMargin m_pageMargin;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_JOB_H_
