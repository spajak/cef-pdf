#ifndef SERVER_REQUEST_HANDLER_H_
#define SERVER_REQUEST_HANDLER_H_

#include "../Client.h"
#include "Http.h"

#include "include/cef_base.h"

namespace cefpdf {
namespace server {

class RequestHandler : public CefBase
{
public:
    RequestHandler(CefRefPtr<cefpdf::Client> client) :
        m_client(client) {};

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    void Handle(const http::Request&, http::Response&);

private:
    CefRefPtr<cefpdf::Client> m_client;

    void SetDate(http::Response&);
    void SetContentLength(http::Response&);

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RequestHandler);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_REQUEST_HANDLER_H_