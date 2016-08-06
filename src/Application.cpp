#include "Application.h"
#include "BrowserHandler.h"
#include "StdInputSchemeHandlerFactory.h"
#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

Application::PaperSizes Application::paperSizes = {
    {"A0",  {841,  1189}},
    {"A1",  {594,   841}},
    {"A2",  {420,   594}},
    {"A3",  {297,   420}},
    {"A4",  {210,   297}},
    {"A5",  {148,   210}},
    {"A6",  {105,   148}},
    {"A7",  {74,    105}},
    {"A8",  {52,     74}},
    {"A9",  {37,     52}},
    {"A10", {26,     37}},

    {"B0",  {1000, 1414}},
    {"B1",  {707,  1000}},
    {"B2",  {500,   707}},
    {"B3",  {353,   500}},
    {"B4",  {250,   353}},
    {"B5",  {176,   250}},
    {"B6",  {125,   176}},
    {"B7",  {88,    125}},
    {"B8",  {62,     88}},
    {"B9",  {44,     62}},
    {"B10", {31,     44}},

    {"C0",  {917,  1297}},
    {"C1",  {648,   917}},
    {"C2",  {458,   648}},
    {"C3",  {324,   458}},
    {"C4",  {229,   324}},
    {"C5",  {162,   229}},
    {"C6",  {114,   162}},
    {"C7",  {81,    114}},
    {"C8",  {57,     81}},
    {"C9",  {40,     57}},
    {"C10", {28,     40}},

    {"Letter",  {216, 279}},
    {"Legal",   {216, 356}},
    {"Tabloid", {279, 432}},
    {"Ledger",  {432, 279}},

    {"Junior Legal",      {127, 203}},
    {"Half Letter",       {140, 216}},
    {"Government Letter", {203, 267}},
    {"Government Legal",  {216, 330}},

    {"ANSI A", {216,  279}},
    {"ANSI B", {279,  432}},
    {"ANSI C", {432,  559}},
    {"ANSI D", {559,  864}},
    {"ANSI E", {864, 1118}},

    {"2R",  {64,   89}},
    {"3R",  {89,  127}},
    {"4R",  {102, 152}},
    {"5R",  {127, 178}},
    {"6R",  {152, 203}},
    {"8R",  {203, 254}},
    {"10R", {254, 305}},
    {"11R", {279, 356}},
    {"12R", {305, 381}}
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
            m_pdfSettings.page_width = it->second.first * 1000;
            m_pdfSettings.page_height = it->second.second * 1000;
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

