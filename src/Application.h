#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "include/cef_app.h"

#include "Utils.h"
#include "PrintHandler.h"
#include "BrowserHandler.h"

#include <utility>
#include <unordered_map>

#define VERSION "0.1.1"

class Application : public CefApp,
                    public CefBrowserProcessHandler
{
    public:

    typedef std::unordered_map<
        CefString,
        std::pair<int, int>,
        CIHash, CIEqual
    > PaperSizes;

    static PaperSizes paperSizes;

    Application();
    bool ParseCommandLine(CefRefPtr<CefCommandLine> commandLine);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;
    virtual void OnContextInitialized() OVERRIDE;
    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    private:

    CefRefPtr<CefPrintHandler> m_printHandler;
    void PrintHelp(CefString name);
    CefPdfPrintSettings m_pdfSettings;
    CefString m_defaultEncoding = "utf-8";
    CefString m_urlInput = "stdin://get";
    CefString m_pdfOutput = "output.pdf";
    CefString m_paperSize = "A4";

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Application);
};

#endif // APPLICATION_H_
