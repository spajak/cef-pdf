#ifndef PDF_PRINT_JOB_H_
#define PDF_PRINT_JOB_H_

#include "include/cef_base.h"

class PdfPrintJob : public CefBase
{
    public:

    PdfPrintJob(const CefString& url);

    CefString GetUrl();

    CefString GetOutputPath();

    void SetOutputPath(const CefString& outputPath);

    void SetPaperSize(const CefString& paperSize);

    void SetLandscape(bool flag = true);

    void SetMargin(CefString margin);

    CefRefPtr<ContentProvider> SetContentProvider(CefRefPtr<ContentProvider> contentProvider)
    {
        m_contentProvider = contentProvider;
    }

    CefRefPtr<ContentProvider> GetContentProvider()
    {
        return m_contentProvider;
    };

    // Prepare PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings();

    // Get PDF content from output file
    CefString GetOutput();

    private:

    CefString m_url;
    CefString m_urlInput = "stdin://get";
    CefString m_pdfOutput = "output.pdf";
    CefString m_paperSize = "A4";
    CefRefPtr<ContentProvider> m_contentProvider;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PdfPrintJob);
};

#endif // PDF_PRINT_JOB_H_
