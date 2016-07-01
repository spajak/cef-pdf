#ifndef STD_INPUT_SCHEME_HANDLER_FACTORY_H_
#define STD_INPUT_SCHEME_HANDLER_FACTORY_H_

#include "include/cef_scheme.h"

class StdInputSchemeHandlerFactory : public CefSchemeHandlerFactory
{
    public:

    StdInputSchemeHandlerFactory();

    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERRIDE;

    private:

    CefRefPtr<CefStreamReader> GetStdInputStream();

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInputSchemeHandlerFactory);
};

#endif // STD_INPUT_SCHEME_HANDLER_FACTORY_H_