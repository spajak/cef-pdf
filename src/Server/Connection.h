#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#include "Http.h"

#include "include/cef_base.h"
#include <asio.hpp>

#include <functional>
#include <utility>
#include <vector>
#include <array>

namespace cefpdf {
namespace server {

class ConnectionManager;

class Connection : public CefBase
{
public:
    Connection(
        CefRefPtr<ConnectionManager> connectionManager,
        asio::ip::tcp::socket socket
    ) :
        m_connectionManager(connectionManager),
        m_socket(std::move(socket)) {};

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void Start() {
        ReadSome();
    };

    void Write();

    bool isOpen() {
        return m_socket.is_open();
    };

    void Close() {
        m_socket.close();
    };

    const http::Request& GetRequest() const {
        return m_request;
    };

    http::Response& GetResponse() {
        return m_response;
    };

private:
    void ReadSome();
    void AsyncReadSome(std::error_code, std::size_t);
    void AsyncWrite(std::error_code, std::size_t);
    void ParseRequest();
    std::vector<asio::const_buffer> ResponseToBuffers();

    CefRefPtr<ConnectionManager> m_connectionManager;
    http::Request m_request;
    http::Response m_response;

    asio::ip::tcp::socket m_socket;
    std::array<char, 1024*16> m_buffer;
    std::string m_requestData;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Connection);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CONNECTION_H_