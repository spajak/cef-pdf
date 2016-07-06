#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

PrintHandler::PrintHandler() {}

// CefPrintHandler methods:
// -------------------------------------------------------------------------
CefSize PrintHandler::GetPdfPaperSize(int device_units_per_inch)
{
    DLOG(INFO) << "PrintHandler::GetPdfPaperSize: " << device_units_per_inch;

    return CefSize(device_units_per_inch*100, device_units_per_inch*100*2);
}

bool PrintHandler::OnPrintDialog(bool has_selection, CefRefPtr<CefPrintDialogCallback> callback)
{
    return true;
}

bool PrintHandler::OnPrintJob(const CefString& document_name, const CefString& pdf_file_path, CefRefPtr<CefPrintJobCallback> callback)
{
    return true;
}

void PrintHandler::OnPrintReset()
{

}

void PrintHandler::OnPrintSettings(CefRefPtr<CefPrintSettings> settings, bool get_defaults)
{
    DLOG(INFO) << "OnPrintSettings";
}

void PrintHandler::OnPrintStart(CefRefPtr<CefBrowser> browser)
{

}
