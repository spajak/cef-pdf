#ifndef SERVER_SESSION_H_
#define SERVER_SESSION_H_

#include "../Client.h"
#include "../Job/Job.h"
#include "Http.h"

#include "include/cef_base.h"
#include <asio.hpp>

#include <functional>
#include <utility>
#include <vector>
#include <array>

namespace cefpdf {
namespace server {

class SessionManager;

class Session : public CefBaseRefCounted
{

public:
    Session(
        CefRefPtr<cefpdf::Client>,
        CefRefPtr<SessionManager>,
        asio::ip::tcp::socket socket
    );

    Session(const Session&) = delete;

    Session& operator=(const Session&) = delete;

    void Start() {
        Read();
    }

    void Close() {
        m_socket.close();
    }

    bool IsOpen() {
        return m_socket.is_open();
    }

    const http::Request& GetRequest() const {
        return m_request;
    }

    http::Response& GetResponse() {
        return m_response;
    }

private:
    void Read();

    void Write();

    void OnRead(std::error_code, std::size_t);

    void OnWrite(std::error_code, std::size_t);

    void ParseRequest();

    std::vector<asio::const_buffer> ResponseToBuffers();

    bool Handle();

    void OnResolve(const std::string&, CefRefPtr<cefpdf::job::Job>);

    CefRefPtr<cefpdf::Client> m_client;

    CefRefPtr<SessionManager> m_sessionManager;

    http::Request m_request;

    http::Response m_response;

    asio::ip::tcp::socket m_socket;

    std::array<char, 1024*16> m_buffer;

    std::string m_requestData;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Session)
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SESSION_H_
