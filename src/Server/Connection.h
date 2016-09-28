#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#include "RequestHandler.h"

#include "include/cef_base.h"
#include <asio.hpp>

#include <utility>

namespace cefpdf {
namespace server {

class Connection : public CefBase
{
public:
    Connection(asio::ip::tcp::socket socket, CefRefPtr<RequestHandler> requestHandler) :
        m_socket(std::move(socket)),
        m_requestHandler(requestHandler) {};

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void start();
    void stop();

private:
    asio::ip::tcp::socket m_socket;

    CefRefPtr<RequestHandler> m_requestHandler;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Connection);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CONNECTION_H_