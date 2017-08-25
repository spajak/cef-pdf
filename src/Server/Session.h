#ifndef SERVER_SESSION_H_
#define SERVER_SESSION_H_

#include "../Client.h"
#include "../Job/Job.h"
#include "Http.h"

#include "include/cef_base.h"
#include <asio.hpp>

#include <string>
#include <functional>
#include <utility>

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
        ReadHeaders();
    }

    void Close() {
        m_socket.close();
    }

    bool IsOpen() {
        return m_socket.is_open();
    }

private:
    void ReadHeaders();

    void ReadChunk();

    void Read(std::size_t);

    void ReadAll();

    void Write();

    void Write(const std::string&);

    void OnReadHeaders(const std::error_code&, std::size_t);

    void OnReadChunk(const std::error_code&, std::size_t);

    void OnRead(const std::error_code&, std::size_t);

    void ParseRequestHeaders();

    void Handle();

    void HandlePDF(const std::string&);

    void OnResolve(CefRefPtr<cefpdf::job::Job>);

    bool ParseChunks(const std::string&);

    void Write100Continue();

    std::string GetAboutReply();

    std::string GetPageSizesReply();

    std::string FetchBuffer();

    CefRefPtr<cefpdf::Client> m_client;

    CefRefPtr<SessionManager> m_sessionManager;

    http::Request m_request;

    http::Response m_response;

    asio::ip::tcp::socket m_socket;

    asio::streambuf m_buffer;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Session)
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SESSION_H_
