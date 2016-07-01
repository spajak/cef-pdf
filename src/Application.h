#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "include/cef_app.h"

class Application : public CefApp,
                    public CefBrowserProcessHandler
{
    public:

    Application();
    Application(CefRefPtr<CefCommandLine> commandLine);

    CefString FetchStdInput();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

    private:

    CefRefPtr<CefCommandLine> m_commandLine;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Application);
};

#endif // APPLICATION_H_
