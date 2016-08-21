#ifndef CLIENT_H_
#define CLIENT_H_

#include "include/cef_app.h"

#include "Utils.h"
#include "PrintHandler.h"
#include "BrowserHandler.h"

#include <utility>
#include <unordered_map>

#define VERSION "0.1.5"

class Client : public CefApp,
               public CefBrowserProcessHandler
{
    public:

    typedef std::unordered_map<
        CefString,
        std::pair<int, int>,
        CIHash, CIEqual
    > PaperSizes;

    static PaperSizes paperSizes;

    Client();

    void Initialize();
    void PostPrintJob(PdfPrintJob printJob);
    void Shutdown();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;
    virtual void OnContextInitialized() OVERRIDE;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    private:

    CefRefPtr<CefPrintHandler> m_printHandler;

    CefSettings m_settings;
    CefWindowInfo m_windowInfo;
    CefBrowserSettings m_browserSettings;

    CefPdfPrintSettings m_pdfSettings;
    CefString m_defaultEncoding = "utf-8";
    CefString m_urlInput = "stdin://get";
    CefString m_pdfOutput = "output.pdf";
    CefString m_paperSize = "A4";

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

#endif // CLIENT_H_
