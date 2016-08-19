#include "Client.h"
#include "StdInputSchemeHandlerFactory.h"
#include "BrowserHandler.h"
#include "RenderHandler.h"
#include "PrintHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>

Client::Client()
{
    m_browserHandler = new BrowserHandler;
    m_renderHandler = new RenderHandler;
    m_printHandler = new PrintHandler;

    //m_settings.single_process = true;
    m_settings.no_sandbox = true;
    m_settings.windowless_rendering_enabled = true;
    m_settings.command_line_args_disabled = false;

    m_windowInfo.windowless_rendering_enabled = true;
    m_windowInfo.transparent_painting_enabled = false;

    m_browserSettings.windowless_frame_rate = 1;
    CefString(&m_browserSettings.default_encoding).FromString(m_defaultEncoding);
    m_browserSettings.javascript_open_windows = STATE_DISABLED;
    m_browserSettings.javascript_close_windows = STATE_DISABLED;
    m_browserSettings.plugins = STATE_DISABLED;
}

void Client::Initialize()
{
    CefMainArgs mainArgs;
    // Initialize CEF in the main process.
    CefInitialize(mainArgs, m_settings, this, NULL);
}

void Client::PostPrintJob(const CefString& pdfOutput, CefPdfPrintSettings pdfSettings)
{

}

void Client::Shutdown()
{
    // Shut down CEF.
    CefShutdown();
}

// CefApp methods:
// -------------------------------------------------------------------------
CefRefPtr<CefBrowserProcessHandler> Client::GetBrowserProcessHandler()
{
    return this;
}

// CefBrowserProcessHandler methods:
// -------------------------------------------------------------------------
CefRefPtr<CefPrintHandler> Client::GetPrintHandler()
{
    return m_printHandler;
}

void Client::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    DLOG(INFO) << "OnBeforeChildProcessLaunch: " << command_line->GetCommandLineString().ToString() ;
}

void Client::OnContextInitialized()
{
    DLOG(INFO) << "OnContextInitialized";

    CEF_REQUIRE_UI_THREAD();

    // Register scheme handler factory for "stdin" scheme. Allows response for url
    // stdin://get to be generated from user provided standard input.
    CefRefPtr<StdInputSchemeHandlerFactory> factory(new StdInputSchemeHandlerFactory);
    CefRegisterSchemeHandlerFactory("stdin", "get", factory.get());

    // Create the browser window.
    CefBrowserHost::CreateBrowser(m_windowInfo, this, m_urlInput, m_browserSettings, NULL);
}

// CefClient methods:
// -------------------------------------------------------------------------
CefRefPtr<CefLifeSpanHandler> Client::GetLifeSpanHandler()
{
    return m_browserHandler;
}

CefRefPtr<CefLoadHandler> Client::GetLoadHandler()
{
    return m_browserHandler;
}

CefRefPtr<CefRenderHandler> Client::GetRenderHandler()
{
    return m_renderHandler;
}

Client::PaperSizes Client::paperSizes = {
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
