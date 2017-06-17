#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

PrintHandler::PrintHandler() {}

// CefPrintHandler methods:
// -------------------------------------------------------------------------
CefSize PrintHandler::GetPdfPaperSize(CefRefPtr<CefBrowser> browser, int device_units_per_inch)
{
    DLOG(INFO) << "PrintHandler::GetPdfPaperSize: " << device_units_per_inch;

    return CefSize(device_units_per_inch*100, device_units_per_inch*100*2);
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