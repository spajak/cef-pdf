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

void Session::ReadHeaders()
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

void Session::ReadExactly(std::size_t bytes)
{
    asio::async_read(
        m_socket,
        m_buffer,
        asio::transfer_exactly(bytes),
        std::bind(
            &Session::OnRead,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Session::ReadAll()
{
    asio::async_read(
        m_socket,
        m_buffer,
        asio::transfer_all(),
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

    if (m_request.method.empty()) {
        ParseRequestHeaders();

        if (m_request.method == "POST") {
            if (stringsEqual(m_request.expect, "100-continue")) {
                Write100Continue();
            }

            if (m_request.contentLength > 0) {
                std::size_t bytes_to_read = m_request.contentLength - m_request.content.size();
                if (bytes_to_read > 0) {
                    ReadExactly(bytes_to_read);
                } else {
                    HandlePOST();
                }
            } else {
                // No content length: read until connection closes
                ReadAll();
            }

        } else if (m_request.method == "GET") {
            HandleGET();
        } else {
            m_response.SetStatus(http::statuses::badMethod);
            Write();
        }
    } else {
        m_request.content += FetchBuffer();
        HandlePOST();
    }
}

std::string Session::FetchBuffer()
{
    std::ostringstream ss;
    ss << &m_buffer;

    return ss.str();
}

void Session::Write100Continue()
{
    std::error_code error;
    asio::streambuf buffer;
    std::ostream os(&buffer);

    os << http::protocol << " " << http::statuses::cont << http::crlf << http::crlf;

    asio::write(m_socket, buffer, error);
}

void Session::ParseRequestHeaders()
{
    auto requestData = FetchBuffer();

    // Separate headers section from request content
    std::string headerData;
    auto sepIndex = requestData.find(http::crlf + http::crlf);

    if (sepIndex == std::string::npos) {
        headerData = requestData;
    } else {
        headerData = requestData.substr(0, sepIndex + 2);
        m_request.content = requestData.substr(sepIndex + 4, requestData.size() - sepIndex + 4);
    }

    // Parse status line
    std::regex re("^([A-Z]+) (\\S+) HTTP/(.+)" + http::crlf);
    std::smatch match;

    if (std::regex_search(headerData, match, re)) {
        m_request.method = match[1];
        m_request.url = match[2];
        m_request.version = match[3];
    }

    m_request.contentLength = 0;

    // Collect request headers
    std::regex re2("(.+): (.+)" + http::crlf);
    std::string::const_iterator it, end;
    it = match[0].second;
    end = headerData.end();

    while (std::regex_search(it, end, match, re2)) {
        m_request.headers.push_back({match[1], match[2]});

        if (stringsEqual(match[1], http::headers::encoding)) {
            m_request.transferEncoding = match[2];
        } else if (stringsEqual(match[1], http::headers::length)) {
            m_request.contentLength = std::stoi(match[2]);
        } else if (stringsEqual(match[1], http::headers::expect)) {
            m_request.expect = match[2];
        } else if (stringsEqual(match[1], http::headers::location)) {
            m_request.location = match[2];
        } else if (stringsEqual(match[1], http::headers::pageSize)) {
            m_request.pageSize = match[2];
        } else if (stringsEqual(match[1], http::headers::pageMargin)) {
            m_request.pageMargin = match[2];
        } else if (stringsEqual(match[1], http::headers::pdfOptions)) {
            m_request.pdfOptions = match[2];
        }

        it = match[0].second;
    }
}

void Session::HandleGET()
{
    m_response.SetStatus(http::statuses::ok);
    m_response.SetHeader(http::headers::date, formatDate());

    if (m_request.url == "/" || m_request.url == "/about") {
        m_response.SetContent(
            "{\"status\":\"ok\",\"version\":\"" + cefpdf::constants::version + "\"}",
            "application/json"
        );
    } else {
        m_response.SetStatus(http::statuses::notFound);
    }

    Write();
}

void Session::HandlePOST()
{
    m_response.SetStatus(http::statuses::ok);
    m_response.SetHeader(http::headers::date, formatDate());

    // Parse url path
    std::regex re("([^/]+\\.pdf)($|[^\\w])", std::regex_constants::icase);
    std::smatch match;

    if (!std::regex_search(m_request.url, match, re)) {
        m_response.SetStatus(http::statuses::notFound);
        Write();
        return;
    }

    std::string fileName = match[1];

    m_response.SetHeader(http::headers::disposition, "inline; filename=\"" + fileName + "\"");

    CefRefPtr<cefpdf::job::Job> job;

    if (m_request.location.empty()) {
        job = new cefpdf::job::Local(m_request.content);
    } else {
        job = new cefpdf::job::Remote(m_request.location);
    }

    if (!m_request.pageSize.empty()) {
        try {
            job->SetPageSize(m_request.pageSize);
        } catch (...) {}
    }

    if (!m_request.pageMargin.empty()) {
        try {
            job->SetPageMargin(m_request.pageMargin);
        } catch (...) {}
    }

    if (!m_request.pdfOptions.empty()) {
        if (std::string::npos != m_request.pdfOptions.find("landscape")) {
            job->SetLandscape(true);
        }

        if (std::string::npos != m_request.pdfOptions.find("backgrounds")) {
            job->SetBackgrounds(true);
        }
    }

    job->SetCallback(std::bind(
        &Session::OnResolve,
        this,
        std::placeholders::_1
    ));

    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::AddJob, m_client, job));
}

void Session::OnResolve(CefRefPtr<cefpdf::job::Job> job)
{
    if (!m_socket.is_open()) {
        return;
    }

    if (job->GetStatus() == cefpdf::job::Job::Status::SUCCESS) {
        m_response.SetContent(loadTempFile(job->GetOutputPath()), "application/pdf");
    } else {
        m_response.SetStatus(http::statuses::error);
    }

    Write();
}

} // namespace server
} // namespace cefpdf
