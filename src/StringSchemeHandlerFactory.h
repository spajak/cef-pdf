#ifndef STRINGSCHEMEHANDLERFACTORY_H
#define STRINGSCHEMEHANDLERFACTORY_H

#include "include/cef_scheme.h"

class StringSchemeHandlerFactory : public CefSchemeHandlerFactory
{
    public:

    StringSchemeHandlerFactory(const CefString& content);

    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERLOAD;

    private:

    CefString m_content;
}


#endif // STRINGSCHEMEHANDLERFACTORY_H