#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "include/cef_app.h"
#include <utility>
#include <unordered_map>

class Application : public CefApp,
                    public CefBrowserProcessHandler
{
    public:

    template <class T> struct PaperSizeHash;
    template <class T> struct PaperSizeEqKey;

    typedef std::unordered_map<
        CefString,
        std::pair<int, int>,
        PaperSizeHash<CefString>,
        PaperSizeEqKey<CefString>
    > PaperSizes;

    static PaperSizes paperSizes;

    Application(CefRefPtr<CefCommandLine> commandLine);

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

    virtual CefRefPtr<CefPrintHandler> GetPrintHandler() OVERRIDE;

    private:

    void CreatePDF();

    CefRefPtr<CefCommandLine> m_commandLine;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Application);
};

#endif // APPLICATION_H_
