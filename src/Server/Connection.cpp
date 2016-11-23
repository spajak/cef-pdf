#include "Connection.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <string>
#include <functional>
#include <regex>

namespace cefpdf {
namespace server {

using namespace std::placeholders;

void Connection::Start(Callback callback)
{
    Read(callback);
}

void Connection::Read(Callback callback)
{
    m_socket.async_read_some(
        asio::buffer(m_buffer),
        std::bind(&Connection::AsyncReadSome, this, _1, _2, callback)
    );
}

void Connection::Write(const http::Response& response)
{
    m_response = response;

    asio::async_write(
        m_socket,
        ResponseToBuffers(m_response)
        std::bind(&Connection::AsyncWrite, this, _1, _2)
    );
}

void Connection::AsyncReadSome(std::error_code ec, std::size_t bytes_transferred, Callback callback)
{
    if (ec) {
        return;
    }

    m_data += std::string(m_buffer.data(), bytes_transferred);

    if (m_socket.available() > 0) {
        Read(callback);
    } else {
        callback(CreateRequest(m_data));
    }
}

void Connection::AsyncWrite(std::error_code ec, std::size_t bytes_transferred)
{
    if (!ec) {
        // Initiate graceful connection closure.
        asio::error_code ignored_ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if (ec != asio::error::operation_aborted) {
        Close();
    }
}

http::Request Connection::CreateRequest(const std::string& requestData)
{
    http::Request request;

    // Parse status line
    std::regex re("^([A-Z]+) (\\S+) HTTP/(.+)\r\n");
    std::smatch match;

    if (std::regex_search(requestData, match, re)) {
        request.method = match[1];
        request.url = match[2];
        request.version = match[3];
    }

    // Separate headers section from request content
    std::string headerData;
    auto sepIndex = requestData.find(http::crlf + http::crlf);

    if (sepIndex == std::string::npos) {
        headerData = requestData;
        request.content = "";
    } else {
        headerData = requestData.substr(0, sepIndex);
        request.content = requestData.substr(sepIndex + 4, requestData.size() - sepIndex + 4);
    }

    // Collect request headers
    std::regex re2("(.+): (.+)\r\n");
    std::string::const_iterator it, end;
    it = match[0].second;
    end = requestData.end();

    while (std::regex_search(it, end, match, re2)) {
        request.headers.push_back({match[1], match[2]});
        it = match[0].second;
    }

    return request;
}

std::vector<asio::const_buffer> Connection::ResponseToBuffers(const http::Response& response)
{
    std::vector<asio::const_buffer> buffers;

    buffers.push_back(asio::buffer(response.status));
    buffers.push_back(asio::buffer(http::crlf));

    for (std::size_t i = 0; i < response.headers.size(); ++i) {
        buffers.push_back(asio::buffer(response.headers[i].name));
        buffers.push_back(asio::buffer(http::hsep));
        buffers.push_back(asio::buffer(response.headers[i].value));
        buffers.push_back(asio::buffer(http::crlf));
    }

    buffers.push_back(asio::buffer(http::crlf));
    buffers.push_back(asio::buffer(response.content));

    return buffers;
}

} // namespace server
} // namespace cefpdf
