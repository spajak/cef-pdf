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

Application::Application()
{
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

    CefRefPtr<BrowserHandler> handler(new BrowserHandler(m_pdfOutput, m_pdfSettings));

    // Information used when creating the native window.
    CefWindowInfo windowInfo;
    windowInfo.windowless_rendering_enabled = true;
    windowInfo.transparent_painting_enabled = false;

    // Specify CEF browser settings here.
    CefBrowserSettings browserSettings;
    browserSettings.windowless_frame_rate = 1;
    CefString(&browserSettings.default_encoding).FromString(m_defaultEncoding);
    browserSettings.javascript_open_windows = STATE_DISABLED;
    browserSettings.javascript_close_windows = STATE_DISABLED;
    browserSettings.plugins = STATE_DISABLED;

    // Create the browser window.
    CefBrowserHost::CreateBrowser(windowInfo, handler.get(), m_urlInput, browserSettings, NULL);
}

bool Application::ParseCommandLine(CefRefPtr<CefCommandLine> commandLine)
{
    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        PrintHelp(::GetProgramName(commandLine->GetProgram()));
        return false;
    }

    m_pdfSettings.backgrounds_enabled = true;
    m_pdfSettings.landscape = false;

    try {
        if (commandLine->HasSwitch("paper-size")) {
            m_paperSize = commandLine->GetSwitchValue("paper-size");
            if (m_paperSize.empty()) {
                throw "Paper size not specified";
            }
        }

        PaperSizes::iterator it = paperSizes.find(m_paperSize);
        if (it != paperSizes.end()) {
            m_pdfSettings.page_width = it->second.first;
            m_pdfSettings.page_height = it->second.second;
        } else {
            throw "Paper size \"" + m_paperSize.ToString() +"\" is not defined";
        }
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return false;
    }

    if (commandLine->HasSwitch("landscape")) {
        m_pdfSettings.landscape = true;
    }

    if (commandLine->HasArguments()) {
        std::vector<CefString> args;
        commandLine->GetArguments(args);
        if (args.size() > 0) {
            // Get input url
            m_urlInput = args[0];
            if (args.size() > 1) {
                // Get output filename
                m_pdfOutput = args[1];
            }
        }
    }

    return true;
}

void Application::PrintHelp(CefString name)
{
    std::cout << name.ToString() << " v" << VERSION << std::endl;
    std::cout << "  Creates PDF files from HTML pages" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  cef-pdf [options] [input] [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h            This help screen." << std::endl;
    std::cout << "  --paper-size <size>  Size (format) of the paper: A3, B2.. Default is A4." << std::endl;
    std::cout << "  --landscape          Wheather to print with a landscape page orientation. Default is portrait" << std::endl;
    std::cout << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << "  URL to load, may be http, file, data, anything supported by Chrome." << std::endl;
    std::cout << "  Special url stdin:// scheme is used for inputing html from standard input. Default is stdin://get" << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  PDF file name. Standard output is not supported. Defaults to output.pdf" << std::endl;
    std::cout << std::endl;
}

