#ifndef PDF_APPLICATION_H_
#define PDF_APPLICATION_H_

#include "include/cef_app.h"

class PDFApplication : public CefApp,
                       public CefBrowserProcessHandler
{
    public:

    PDFApplication();
    PDFApplication(CefRefPtr<CefCommandLine> commandLine);

    CefString FetchStdInput();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

    private:

    CefRefPtr<CefCommandLine> m_commandLine;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PDFApplication);
};

#endif // PDF_APPLICATION_H_
