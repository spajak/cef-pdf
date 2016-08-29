#ifndef JOB_H_
#define JOB_H_

#include "Common.h"

#include "include/cef_load_handler.h"

namespace cefpdf {

class Job : public CefBase
{
    public:

    Job();
    Job(const CefString& url);

    const CefString& GetUrl();

    void SetUrl(const CefString& url);

    const CefString& GetContent();

    void SetContent(const CefString& content);

    const CefString& GetOutputPath();

    void SetOutputPath(const CefString& outputPath);


    void SetPageSize(const CefString& pageSize);

    void SetLandscape(bool flag = true);

    void SetPageMargin(const CefString& margin);


    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings();

    // Get PDF content from output file
    CefString GetOutputContent();

    private:

    void ParseCustomPageSize(const CefString& pageSize);

    void ParseCustomPageMargin(const CefString& pageMargin);

    CefString m_url;
    CefString m_content;
    CefString m_outputPath;

    PageSize m_pageSize;
    PageOrientation m_pageOrientation = PageOrientation::PORTRAIT;
    PageMargin m_pageMargin;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job);
};

} // namespace cefpdf

#endif // JOB_H_
