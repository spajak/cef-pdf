#ifndef PDF_PRINT_JOB_H_
#define PDF_PRINT_JOB_H_

#include "include/cef_base.h"

class PdfPrintJob : public CefBase
{
    public:

    PdfPrintJob();

    private:

    const CefString& m_pdfOutput;
    CefPdfPrintSettings m_pdfSettings;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PdfPrintJob);
};

#endif // PDF_PRINT_JOB_H_
