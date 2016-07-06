#ifndef PRINT_HANDLER_H_
#define PRINT_HANDLER_H_

#include "include/cef_print_handler.h"

class PrintHandler : public CefPrintHandler
{
    public:

    PrintHandler();

    // CefPrintHandler methods:
    virtual CefSize GetPdfPaperSize(int device_units_per_inch) OVERRIDE;

    virtual bool OnPrintDialog(bool has_selection, CefRefPtr<CefPrintDialogCallback> callback) OVERRIDE;

    virtual bool OnPrintJob(const CefString& document_name, const CefString& pdf_file_path, CefRefPtr<CefPrintJobCallback> callback) OVERRIDE;

    virtual void OnPrintReset() OVERRIDE;

    virtual void OnPrintSettings(CefRefPtr<CefPrintSettings> settings, bool get_defaults) OVERRIDE;

    virtual void OnPrintStart(CefRefPtr<CefBrowser> browser) OVERRIDE;

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PrintHandler);
};

#endif // PRINT_HANDLER_H_
