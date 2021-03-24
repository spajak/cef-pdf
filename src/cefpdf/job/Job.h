// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_JOB_JOB_H_
#define CEFPDF_JOB_JOB_H_

#include <string>

#include "../Common.h"
#include "Visitor.h"

#include "include/cef_base.h"

namespace cefpdf {
namespace job {

class Job : public CefBaseRefCounted
{

public:
    Job();

    virtual void accept(CefRefPtr<Visitor> visitor) = 0;

    const CefString& GetOutputPath() const;
    void SetOutputPath(const CefString& outputPath);

    // Set/get error text
    void SetError(const CefString&);
    bool HasError();
    const CefString& GetError();

    void SetPageSize(const CefString& pageSize);
    void SetLandscape(bool flag = true);
    void SetPageMargin(const CefString& pageMargin);
    void SetBackgrounds(bool flag = true);
    void SetJavaScript(bool flag = true);
    bool GetJavaScript();
    void SetScale(int scale);
    void SetDelay(int delay);
    int GetDelay();

    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings() const;

private:
    CefString m_outputPath;
    PageSize m_pageSize;
    PageOrientation m_pageOrientation;
    PageMargin m_pageMargin;
    bool m_backgrounds;
    bool m_javascript;
    int m_scale;
    int m_delay;
    CefString m_error;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job);
};

} // namespace job
} // namespace cefpdf

#endif // CEFPDF_JOB_JOB_H_
