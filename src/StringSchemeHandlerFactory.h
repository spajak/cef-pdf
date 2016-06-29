#ifndef STRINGSCHEMEHANDLERFACTORY_H_
#define STRINGSCHEMEHANDLERFACTORY_H_

#include "include/cef_scheme.h"

class StringSchemeHandlerFactory : public CefSchemeHandlerFactory
{
    public:

    StringSchemeHandlerFactory();
    StringSchemeHandlerFactory(const CefString& content);

    void SetString(const CefString& content);

    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERRIDE;

    private:

    CefString m_content;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StringSchemeHandlerFactory);
};

#endif // STRINGSCHEMEHANDLERFACTORY_H_