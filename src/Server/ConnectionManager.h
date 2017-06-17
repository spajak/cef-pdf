#ifndef SERVER_CONNECTION_MANAGER_H_
#define SERVER_CONNECTION_MANAGER_H_

#include "Connection.h"
#include "RequestHandler.h"

#include "include/cef_base.h"

#include <set>

namespace cefpdf {
namespace server {

class ConnectionManager : public CefBaseRefCounted
{

public:
    ConnectionManager(CefRefPtr<RequestHandler> requestHandler) :
        m_requestHandler(requestHandler) {};

    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    void Start(CefRefPtr<Connection>);
    void Stop(CefRefPtr<Connection>);
    void StopAll();

    void OnRequest(CefRefPtr<Connection>);

private:
    std::set<CefRefPtr<Connection>> m_connections;
    CefRefPtr<RequestHandler> m_requestHandler;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ConnectionManager);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CONNECTION_MANAGER_H_
