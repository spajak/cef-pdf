#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

PrintHandler::PrintHandler() {}

// CefPrintHandler methods:
// -------------------------------------------------------------------------
CefSize PrintHandler::GetPdfPaperSize(int device_units_per_inch)
{
    DLOG(INFO)
        << "PrintHandler::GetPdfPaperSize"
        << " with device_units_per_inch: " << device_units_per_inch;

    // Reportedly this must be implemented in order for Linux to be supported
    // but its implementation won't affect the output when page_width and
    // page_height are set in CefPrintSettings which they always should be.
    return CefSize(1, 1);
}

bool PrintHandler::OnPrintDialog(
    CefRefPtr<CefBrowser> browser,
    bool has_selection,
    CefRefPtr<CefPrintDialogCallback> callback
) {
    return true;
}

bool PrintHandler::OnPrintJob(
    CefRefPtr<CefBrowser> browser,
    const CefString& document_name,
    const CefString& pdf_file_path,
    CefRefPtr<CefPrintJobCallback> callback
) {
    return true;
}

void PrintHandler::OnPrintReset(CefRefPtr<CefBrowser> browser)
{

}

void PrintHandler::OnPrintSettings(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefPrintSettings> settings,
    bool get_defaults
) {

}

void PrintHandler::OnPrintStart(CefRefPtr<CefBrowser> browser)
{

}

} // namespace cefpdf
