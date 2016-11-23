#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#include "Http.h"

#include "include/cef_base.h"
#include <asio.hpp>

#include <functional>
#include <utility>
#include <vector>
#include <array>
#include <iostream>

namespace cefpdf {
namespace server {

class Connection : public CefBase
{
public:
    typedef std::function<void (http::Request)> Callback;

    Connection(asio::ip::tcp::socket socket) :
        m_socket(std::move(socket)) {};

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void Start(Callback callback);

    void Read(Callback callback);

    void Write(const http::Response&);

    void Close() {
        m_socket.close();
    };

    bool isOpen() {
        return m_socket.is_open();
    };

private:
    void AsyncReadSome(std::error_code, std::size_t, Callback);
    void AsyncWrite(std::error_code, std::size_t);

    http::Request CreateRequest(const std::string&);
    std::vector<asio::const_buffer> ResponseToBuffers(const http::Response&);

    asio::ip::tcp::socket m_socket;
    std::array<char, 1024*16> m_buffer;
    std::string m_data;
    std::vector<asio::const_buffer> m_writeBuffers;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Connection);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CONNECTION_H_