#ifndef PRINT_HANDLER_H_
#define PRINT_HANDLER_H_

#include "include/cef_print_handler.h"

namespace cefpdf {

class PrintHandler : public CefPrintHandler
{
    public:

    PrintHandler();

    // CefPrintHandler methods:
    virtual CefSize GetPdfPaperSize(int device_units_per_inch) override;

    virtual bool OnPrintDialog(bool has_selection, CefRefPtr<CefPrintDialogCallback> callback) override;

    virtual bool OnPrintJob(const CefString& document_name, const CefString& pdf_file_path, CefRefPtr<CefPrintJobCallback> callback) override;

    virtual void OnPrintReset() override;

    virtual void OnPrintSettings(CefRefPtr<CefPrintSettings> settings, bool get_defaults) override;

    virtual void OnPrintStart(CefRefPtr<CefBrowser> browser) override;

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PrintHandler);
};

} // namespace cefpdf

#endif // PRINT_HANDLER_H_
