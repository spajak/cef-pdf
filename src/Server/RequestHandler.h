#ifndef SERVER_REQUEST_HANDLER_H_
#define SERVER_REQUEST_HANDLER_H_

#include "../Client.h"
#include "Http.h"

#include "include/cef_base.h"

namespace cefpdf {
namespace server {

class RequestHandler : public CefBaseRefCounted
{

public:
    RequestHandler(CefRefPtr<cefpdf::Client>);

    void Handle(const http::Request&, http::Response&);

private:
    CefRefPtr<cefpdf::Client> m_client;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RequestHandler)
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_REQUEST_HANDLER_H_
