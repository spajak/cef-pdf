#include "Session.h"
#include "SessionManager.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"
#include "../Common.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>
#include <functional>
#include <regex>

namespace cefpdf {
namespace server {

Session::Session(
    CefRefPtr<cefpdf::Client> client,
    CefRefPtr<SessionManager> sessionManager,
    asio::ip::tcp::socket socket
) :
    m_client(client),
    m_sessionManager(sessionManager),
    m_socket(std::move(socket))
{}

void Session::Read()
{
    asio::async_read_until(
        m_socket,
        m_buffer,
        http::crlf + http::crlf,
        std::bind(
            &Session::OnRead,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Session::Write()
{
    std::error_code error;
    asio::streambuf buffer;
    std::ostream os(&buffer);

    m_response.WriteToStream(os);

    asio::write(m_socket, buffer, error);

    if (!error) {
        // Initiate graceful connection closure.
        asio::error_code ignored_ec;
        m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }

    if (error != asio::error::operation_aborted) {
        m_sessionManager->Stop(this);
    }
}

void Session::OnRead(std::error_code ec, std::size_t bytes_transferred)
{
    if (ec) {
        return;
    }

    std::ostringstream ss;
    ss << &m_buffer;
    m_requestData += ss.str();

    if (m_socket.available() > 0) {
        asio::async_read(
            m_socket,
            m_buffer,
            asio::transfer_exactly(m_socket.available()),
            std::bind(
                &Session::OnRead,
                this,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    } else {
        ParseRequest();
        if (!Handle()) {
            Write();
        }
    }
}

void Session::ParseRequest()
{
    // Separate headers section from request content
    std::string headerData;
    auto sepIndex = m_requestData.find(http::crlf + http::crlf);

    if (sepIndex == std::string::npos) {
        headerData = m_requestData;
    } else {
        headerData = m_requestData.substr(0, sepIndex + 2);
        m_request.content = m_requestData.substr(sepIndex + 4, m_requestData.size() - sepIndex + 4);
    }

    // Parse status line
    std::regex re("^([A-Z]+) (\\S+) HTTP/(.+)" + http::crlf);
    std::smatch match;

    if (std::regex_search(headerData, match, re)) {
        m_request.method = match[1];
        m_request.url = match[2];
        m_request.version = match[3];
    }

    // Collect request headers
    std::regex re2("(.+): (.+)" + http::crlf);
    std::string::const_iterator it, end;
    it = match[0].second;
    end = headerData.end();

    while (std::regex_search(it, end, match, re2)) {
        m_request.headers.push_back({match[1], match[2]});
        it = match[0].second;
    }
}

bool Session::Handle()
{
    m_response.SetStatus(http::statuses::ok);
    m_response.SetHeader(http::headers::date, formatDate());

    if (!(m_request.method == "GET" || m_request.method == "POST")) {
        m_response.SetStatus(http::statuses::badMethod);
        return false;
    }

    if (m_request.url == "/" || m_request.url == "/about") {
        m_response.SetContent(
            "{\"status\":\"ok\",\"version\":\"" + cefpdf::constants::version + "\"}",
            "application/json"
        );
        return false;
    }

    // Parse url path
    std::regex re("([^/]+\\.pdf)($|[^\\w])", std::regex_constants::icase);
    std::smatch match;

    if (!std::regex_search(m_request.url, match, re)) {
        m_response.SetStatus(http::statuses::notFound);
        return false;
    }

    std::string fileName = match[1];

    m_response.SetHeader(http::headers::disposition, "inline; filename=\"" + fileName + "\"");

    std::string location;

    for (auto const &h: m_request.headers) {
        if (h.name.compare(http::headers::location) == 0) {
            location = h.value;
            break;
        }
    }

    CefRefPtr<cefpdf::job::Job> job;

    if (location.empty()) {
        if (m_request.method != "POST") {
            m_response.SetStatus(http::statuses::badMethod);
            return false;
        }

        if (m_request.content.empty()) {
            m_response.SetStatus(http::statuses::badRequest);
            return false;
        }

        job = new cefpdf::job::Local(m_request.content);
    } else {
        job = new cefpdf::job::Remote(location);
    }

    job->SetCallback(std::bind(
        &Session::OnResolve,
        this,
        std::placeholders::_1,
        std::placeholders::_2
    ));

    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::AddJob, m_client, job));
    return true;
}

void Session::OnResolve(const std::string& result, CefRefPtr<cefpdf::job::Job> job)
{
    if (!m_socket.is_open()) {
        return;
    }

    if (result == "success") {
        m_response.SetContent(loadTempFile(job->GetOutputPath()), "application/pdf");
    } else {
        m_response.SetStatus(http::statuses::error);
    }

    Write();
}

} // namespace server
} // namespace cefpdf
