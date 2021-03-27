// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_PRINT_JOB_H_
#define CEFPDF_PRINT_JOB_H_

#include "Common.h"

#include "include/cef_base.h"
#include "include/cef_stream.h"

namespace cefpdf {

class PrintJob : public CefBaseRefCounted
{

public:
    enum struct Status {PENDING, LOADING, PRINTING, SUCCESS, FAILED};

    enum struct ErrorCode {ABORTED, NETWORK, HTTP, PRINT};

    struct Error {
        ErrorCode code;
        CefString message;
    };

    explicit PrintJob(const CefString& url);

    void SetPageSize(const CefString& pageSize);
    void SetLandscape(bool flag = true);
    void SetPageMargin(const CefString& pageMargin);
    void SetBackgrounds(bool flag = true);
    void SetJavaScript(bool flag = true);
    bool GetJavaScript();
    void SetScale(int scale);
    void SetDelay(int delay);
    int GetDelay();

    const CefString& GetUrl() const;
    CefRefPtr<CefStreamReader> GetStreamReader();
    void SetStreamReader(CefRefPtr<CefStreamReader>);
    const CefString& GetOutputPath() const;
    void SetOutputPath(const CefString& outputPath);
    CefPdfPrintSettings GetCefPdfPrintSettings() const;
    void SetError(const Error&);
    const Error& GetError();
    Status GetStatus();
    void SetStatus(Status);
    bool IsSuccess();
    bool IsFailed();
    bool IsFinished();

private:
    PageSize m_pageSize;
    PageOrientation m_pageOrientation;
    PageMargin m_pageMargin;
    bool m_backgrounds;
    bool m_javascript;
    int m_scale;
    int m_delay;

    CefString m_url;
    CefRefPtr<CefStreamReader> m_streamReader;
    CefString m_outputPath;
    Status m_status;
    Error m_error;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PrintJob);
};

} // namespace cefpdf

#endif // CEFPDF_PRINT_JOB_H_
