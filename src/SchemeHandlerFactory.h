#ifndef SCHEME_HANDLER_FACTORY_H_
#define SCHEME_HANDLER_FACTORY_H_

#include "Job/Manager.h"

#include "include/cef_scheme.h"

namespace cefpdf {

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{

public:
    SchemeHandlerFactory(CefRefPtr<job::Manager> jobsManager);

    // CefSchemeHandlerFactory methods:
    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) override;

private:
    CefRefPtr<job::Manager> m_jobsManager;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};

} // namespace cefpdf

#endif // SCHEME_HANDLER_FACTORY_H_
