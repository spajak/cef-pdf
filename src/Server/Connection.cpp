#include "Connection.h"
#include "ConnectionManager.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <string>
#include <functional>
#include <regex>

namespace cefpdf {
namespace server {

using namespace std::placeholders;

void Connection::ReadSome()
{
    m_socket.async_read_some(
        asio::buffer(m_buffer),
        std::bind(&Connection::AsyncReadSome, this, _1, _2)
    );
}

void Connection::Write()
{
    asio::async_write(
        m_socket,
        ResponseToBuffers(),
        std::bind(&Connection::AsyncWrite, this, _1, _2)
    );
}

void Connection::AsyncReadSome(std::error_code ec, std::size_t bytes_transferred)
{
    if (ec) {
        return;
    }

    m_requestData += std::string(m_buffer.data(), bytes_transferred);

    if (m_socket.available() > 0) {
        ReadSome();
    } else {
        ParseRequest();
        m_connectionManager->OnRequest(this);
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
        m_connectionManager->Stop(this);
    }
}

void Connection::ParseRequest()
{
    // Parse status line
    std::regex re("^([A-Z]+) (\\S+) HTTP/(.+)\r\n");
    std::smatch match;

    if (std::regex_search(m_requestData, match, re)) {
        m_request.method = match[1];
        m_request.url = match[2];
        m_request.version = match[3];
    }

    // Separate headers section from request content
    std::string headerData;
    auto sepIndex = m_requestData.find(http::crlf + http::crlf);

    if (sepIndex == std::string::npos) {
        headerData = m_requestData;
    } else {
        headerData = m_requestData.substr(0, sepIndex);
        m_request.content = m_requestData.substr(sepIndex + 4, m_requestData.size() - sepIndex + 4);
    }

    if (m_request.content.size() == 0) {
        m_request.content = "<!DOCTYPE html>";
    }

    // Collect request headers
    std::regex re2("(.+): (.+)\r\n");
    std::string::const_iterator it, end;
    it = match[0].second;
    end = m_requestData.end();

    while (std::regex_search(it, end, match, re2)) {
        m_request.headers.push_back({match[1], match[2]});
        it = match[0].second;
    }
}

std::vector<asio::const_buffer> Connection::ResponseToBuffers()
{
    std::vector<asio::const_buffer> buffers;

    buffers.push_back(asio::buffer(m_response.status));
    buffers.push_back(asio::buffer(http::crlf));

    for (std::size_t i = 0; i < m_response.headers.size(); ++i) {
        buffers.push_back(asio::buffer(m_response.headers[i].name));
        buffers.push_back(asio::buffer(http::hsep));
        buffers.push_back(asio::buffer(m_response.headers[i].value));
        buffers.push_back(asio::buffer(http::crlf));
    }

    buffers.push_back(asio::buffer(http::crlf));
    buffers.push_back(asio::buffer(m_response.content));

    return buffers;
}

} // namespace server
} // namespace cefpdf
