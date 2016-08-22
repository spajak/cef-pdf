#ifndef PDF_PRINT_JOB_H_
#define PDF_PRINT_JOB_H_

#include "include/cef_base.h"

class PdfPrintJob : public CefBase
{
    public:

    enum struct Type {
        URL, STRING
    };

    struct PaperSize {
        int width, height;
    };

    struct PaperMargin {
        int top, right, bottom, left;
    };

    enum struct PaperOrientation {
        PORTRAIT, LANDSCAPE
    };

    typedef std::unordered_map<
        CefString,
        PaperSize,
        CIHash, CIEqual
    > PaperSizesMap;

    static PaperSizesMap paperSizesMap;

    PdfPrintJob();

    PdfPrintJob(Type type);

    PdfPrintJob(const CefString& url);

    Type GetType();

    const CefString& GetUrl();

    const CefString& GetContent();

    void SetUrl(const CefString&);

    void SetContent(const CefString& content);


    const CefString& GetOutputPath();

    void SetOutputPath(const CefString& outputPath);


    void SetPaperSize(const CefString& paperSize);

    void SetLandscape(bool flag = true);

    void SetMargin(const CefString& margin);




    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings();

    // Get PDF content from output file
    CefString GetOutput();

    private:

    PaperSize parsePaperSize(const CefString& paperSize);

    PaperMargin parseMargin(const CefString& margin);

    Type type = Type::URL;
    CefString m_url;
    CefString m_content;

    CefString m_outputPath;
    CefString m_paperSize = "A4";
    Orientation m_orientation = PaperOrientation::PORTRAIT;
    CefString m_margin;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PdfPrintJob);
};

#endif // PDF_PRINT_JOB_H_
