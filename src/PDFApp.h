#ifndef PDFAPP_H
#define PDFAPP_H

#include "include/cef_app.h"

class PDFApp : public CefApp,
               public CefBrowserProcessHandler
{
    public:

    PDFApp();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PDFApp);
};

#endif // PDFAPP_H
