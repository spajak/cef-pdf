#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

PrintHandler::PrintHandler() {}

std::map<CefString, PrintHandler::PaperSize> PrintHandler::paperSizes = {
    {"A0",  {841000,  1189000}},
    {"A1",  {594000,   841000}},
    {"A2",  {420000,   594000}},
    {"A3",  {297000,   420000}},
    {"A4",  {210000,   297000}},
    {"A5",  {148000,   210000}},
    {"A6",  {105000,   148000}},
    {"A7",  {74000,    105000}},
    {"A8",  {52000,     74000}},
    {"A9",  {37000,     52000}},
    {"A10", {26000,     37000}},

    {"B0",  {1000000, 1414000}},
    {"B1",  {707000,  1000000}},
    {"B2",  {500000,   707000}},
    {"B3",  {353000,   500000}},
    {"B4",  {250000,   353000}},
    {"B5",  {176000,   250000}},
    {"B6",  {125000,   176000}},
    {"B7",  {88000,    125000}},
    {"B8",  {62000,     88000}},
    {"B9",  {44000,     62000}},
    {"B10", {31000,     44000}},

    {"C0",  {917000,  1297000}},
    {"C1",  {648000,   917000}},
    {"C2",  {458000,   648000}},
    {"C3",  {324000,   458000}},
    {"C4",  {229000,   324000}},
    {"C5",  {162000,   229000}},
    {"C6",  {114000,   162000}},
    {"C7",  {81000,    114000}},
    {"C8",  {57000,     81000}},
    {"C9",  {40000,     57000}},
    {"C10", {28000,     40000}},
};


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
    DLOG(INFO) << "PrintHandler::OnPrintSettings";
}

void PrintHandler::OnPrintStart(CefRefPtr<CefBrowser> browser)
{

}
