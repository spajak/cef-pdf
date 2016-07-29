#include "Application.h"
#include "BrowserHandler.h"
#include "StdInputSchemeHandlerFactory.h"
#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

Application::PaperSizes Application::paperSizes = {
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

Application::Application(CefRefPtr<CefCommandLine> commandLine)
{
    m_commandLine = commandLine;
    m_printHandler = new PrintHandler;
}

CefRefPtr<CefBrowserProcessHandler> Application::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefPrintHandler> Application::GetPrintHandler()
{
    return m_printHandler;
}

void Application::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "OnBeforeChildProcessLaunch: " << command_line->GetCommandLineString().ToString() ;
}

void Application::OnContextInitialized()
{
    DLOG(INFO) << "OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    // Register scheme handler factory for "stdin" scheme. Allows response for url
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());

    // Set default string encoding to UTF-8
    CefRefPtr<CefValue> value = CefValue::Create();
    value->SetString("utf-8");
    CefString error;
    CefRequestContext::GetGlobalContext()
        ->SetPreference("intl.charset_default", value, error);

    CefString url = "stdin://get";
    CefString output = "output.pdf";

    if (m_commandLine->HasArguments()) {
        std::vector<CefString> args;
        m_commandLine->GetArguments(args);
        if (args.size() > 0) {
            // Get input url
            url = args[0];
            if (args.size() > 1) {
                // Get output filename
                output = args[1];
            }
        }
    }

    CefPdfPrintSettings pdfSettings;

    try {
        pdfSettings = GetPdfSettings();
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return;
    }

    BrowserHandler::LoadAndSaveToPDF(url, output, pdfSettings);
}

CefPdfPrintSettings Application::GetPdfSettings()
{
    CefPdfPrintSettings pdfSettings;
    pdfSettings.backgrounds_enabled = true;
    pdfSettings.landscape = false;

    CefString paperSize = "A4";

    if (m_commandLine->HasSwitch("paper-size")) {
        paperSize = m_commandLine->GetSwitchValue("paper-size");
        if (paperSize.empty()) {
            throw "Paper size not specified";
        }
    }

    PaperSizes::iterator it = paperSizes.find(paperSize);
    if (it != paperSizes.end()) {
        pdfSettings.page_width = it->second.first;
        pdfSettings.page_height = it->second.second;
    } else {
        throw "Paper size \"" + paperSize.ToString() +"\" is not defined";
    }

    if (m_commandLine->HasSwitch("landscape")) {
        pdfSettings.landscape = true;
    }

    return pdfSettings;
}

