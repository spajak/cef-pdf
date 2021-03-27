// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "LinuxPrintHandler.h"

#include "include/wrapper/cef_helpers.h"

namespace cefpdf {

LinuxPrintHandler::LinuxPrintHandler() {}

// CefPrintHandler methods:
// -------------------------------------------------------------------------
CefSize LinuxPrintHandler::GetPdfPaperSize(int device_units_per_inch)
{
    DLOG(INFO) << "LinuxPrintHandler::GetPdfPaperSize (" << device_units_per_inch << ")";

    return CefSize(device_units_per_inch*100, device_units_per_inch*100*2);
}

bool LinuxPrintHandler::OnPrintDialog(
    CefRefPtr<CefBrowser> browser,
    bool has_selection,
    CefRefPtr<CefPrintDialogCallback> callback
) {
    return true;
}

bool LinuxPrintHandler::OnPrintJob(
    CefRefPtr<CefBrowser> browser,
    const CefString& document_name,
    const CefString& pdf_file_path,
    CefRefPtr<CefPrintJobCallback> callback
) {
    return true;
}

void LinuxPrintHandler::OnPrintReset(CefRefPtr<CefBrowser> browser)
{

}

void LinuxPrintHandler::OnPrintSettings(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefPrintSettings> settings,
    bool get_defaults
) {

}

void LinuxPrintHandler::OnPrintStart(CefRefPtr<CefBrowser> browser)
{

}

} // namespace cefpdf
