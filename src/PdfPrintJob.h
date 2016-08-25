#ifndef PDF_PRINT_JOB_H_
#define PDF_PRINT_JOB_H_

#include "Common.h"

#include "include/cef_load_handler.h"

namespace cefpdf {

class PdfPrintJob : public CefBase
{
    public:

    PdfPrintJob();

    PdfPrintJob(const CefString& url);

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

    void ParsePageSize(const CefString& pageSize);

    void ParsePageMargin(const CefString& pageMargin);

    CefString m_url;
    CefString m_content;
    CefString m_outputPath;

    PageSize m_pageSize;
    PageOrientation m_pageOrientation = PageOrientation::PORTRAIT;
    PageMargin m_pageMargin;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PdfPrintJob);
};

} // namespace cefpdf

#endif // PDF_PRINT_JOB_H_
