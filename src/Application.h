#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "include/cef_app.h"

#include "Utils.h"
#include "PrintHandler.h"
#include "BrowserHandler.h"

#include <utility>
#include <unordered_map>

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

    Application(CefRefPtr<CefCommandLine> commandLine);

    void CreatePDF();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;

    virtual void OnContextInitialized() OVERRIDE;

    virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;


    private:

    CefRefPtr<CefPrintHandler> m_printHandler;

    CefPdfPrintSettings GetPdfSettings();

    CefRefPtr<CefCommandLine> m_commandLine;

    CefString m_defaultEncoding = "utf-8";

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Application);
};

#endif // APPLICATION_H_
