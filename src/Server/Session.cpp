#include "Session.h"
#include "SessionManager.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"
#include "../Common.h"

#include "include/base/cef_callback.h"
#include "include/wrapper/cef_closure_task.h"

#include <regex>
#include <string>
#include <sstream>
#include <ostream>
#include <iostream>
#include <functional>
#include <filesystem>

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
            &Session::OnReadHeaders,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Session::ReadChunk()
{
    asio::async_read_until(
        m_socket,
        m_buffer,
        http::crlf,
        std::bind(
            &Session::OnReadChunk,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void Session::Read(std::size_t bytes)
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

void Session::Write(const std::string& status)
{
    m_response.SetStatus(status);
    Write();
}

void Session::Write100Continue()
{
    std::error_code error;
    asio::streambuf buffer;
    std::ostream os(&buffer);

    os << http::protocol << " " << http::statuses::cont << http::crlf << http::crlf;

    asio::write(m_socket, buffer, error);
}

void Session::OnReadHeaders(const std::error_code& ec, std::size_t bytes_transferred)
{
    ParseRequestHeaders();

    if (m_request.method == "GET" || m_request.method == "HEAD" || m_request.method == "DELETE") {
        Handle();
    } else if (m_request.method == "POST") {
        if (!(m_request.encoding.empty() || m_request.chunked)) {
            // No transfer encoding supported yet
            Write(http::statuses::unsupported);
            return;
        }

        if (stringsEqual(m_request.expect, "100-continue")) {
            Write100Continue();
        }

        if (m_request.chunked) {
            ReadChunk();
            return;
        }

        if (m_request.length > 0) {
            std::size_t bytesToRead = m_request.length - m_request.content.size();
            if (bytesToRead > 0) {
                Read(bytesToRead);
            } else {
                Handle();
            }
        } else if (!m_request.location.empty()) {
            Handle();
        } else {
            Write(http::statuses::lengthRequired);
        }
    } else {
        Write(http::statuses::badMethod);
    }
}

void Session::OnReadChunk(const std::error_code& ec, std::size_t bytes_transferred)
{
    if (ParseChunks(FetchBuffer())) {
        Handle();
    }
}

void Session::OnRead(const std::error_code& ec, std::size_t bytes_transferred)
{
    auto content = FetchBuffer();

    if (m_request.chunked) {
        if (content.size() > http::crlf.size()) {
            m_request.content += content.substr(0, content.size() - http::crlf.size());
        }

        ReadChunk();
        return;
    } else {
        m_request.content += content;
    }

    Handle();
}

std::string Session::FetchBuffer()
{
    std::ostringstream ss;
    ss << &m_buffer;

    return ss.str();
}

bool Session::ParseChunks(const std::string& content)
{
    std::size_t chunkSize, pos = 0;
    auto separatorSize = http::crlf.size();
    auto idx = content.find(http::crlf);

    while (idx != std::string::npos) {
        chunkSize = std::stoi(m_chunkStart + content.substr(pos, idx - pos), nullptr, 16);
        m_chunkStart.clear();

        if (0 == chunkSize) {
            // Zero chunk, this is the end
            return true;
        }

        m_request.content += content.substr(idx + separatorSize, chunkSize);
        pos = idx + chunkSize + 2 * separatorSize;

        if (content.size() == pos) {
            break;
        } else if (content.size() < pos) {
            // Read rest of the chunk data
            Read(pos - content.size());
            return false;
        }

        idx = content.find(http::crlf, pos);

        if (idx == std::string::npos) {
            m_chunkStart = content.substr(pos);
        }
    }

    // Read next chunk
    ReadChunk();
    return false;
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
        m_request.content = requestData.substr(sepIndex + 4);
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
    m_request.chunked = false;
    m_request.download = false;
    m_request.length = 0;

    while (std::regex_search(it, end, match, re2)) {
        m_request.headers.push_back({match[1], match[2]});

        if (stringsEqual(match[1], http::headers::encoding)) {
            if (!stringsEqual(match[2], "identity")) {
                m_request.encoding = match[2];
                if (stringsEqual(m_request.encoding, "chunked")) {
                    m_request.chunked = true;
                }
            }
        } else if (stringsEqual(match[1], http::headers::download)) {
            if (stringsEqual(match[2], "yes")) {
                m_request.download = true;
            } else {
                m_request.download = false;
            }
        } else if (stringsEqual(match[1], http::headers::length)) {
            m_request.length = std::stoi(match[2]);
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

void Session::Handle()
{
    m_response.SetStatus(http::statuses::ok);
    m_response.SetHeader(http::headers::date, formatDate());

    if (m_request.url == "/" || m_request.url == "/about") {
        m_response.SetContent(GetAboutReply(), "application/json");
        Write();
    } else if (m_request.url == "/list-sizes") {
        m_response.SetContent(GetPageSizesReply(), "application/json");
        Write();
    } else {
        // Parse url path
        std::regex re("([^/]+\\.(pdf|png))($|[^\\w])", std::regex_constants::icase);
        std::smatch match;
        std::string path;

        if (std::regex_search(m_request.url, match, re)) {
            if ((m_request.method == "GET" || m_request.method == "HEAD" || m_request.method == "DELETE") && m_request.location.empty()) {
                if (m_sessionManager->m_persistent) {
                    path = m_sessionManager->m_save + m_request.url;
                    if (std::filesystem::is_regular_file(std::filesystem::status(path))) {
                        if (m_request.method == "GET") {
                            m_response.SetHeader(http::headers::disposition, "inline; filename=\"" + std::string(match[1]) + "\"");
                            m_response.SetContent(loadTempFile(path, false), "application/pdf");
                        } else if (m_request.method == "HEAD") {
                            if (std::string::npos == m_request.url.find(".pdf")) {
                                m_response.SetHeader(cefpdf::server::http::headers::type, "image/png");
                            } else {
                                m_response.SetHeader(cefpdf::server::http::headers::type, "application/pdf");
                            }

                            size_t fileSize =  std::filesystem::file_size(std::filesystem::path(path));
                            m_response.SetHeader(cefpdf::server::http::headers::length, std::to_string(fileSize));
                        } else {
                            if (std::filesystem::remove(std::filesystem::path(path))) {
                                m_response.SetContent("{\"status\":\"ok\"}", "application/json");
                            } else {
                                m_response.SetContent("{\"status\":\"fail\"}", "application/json");
                            }
                        }

                        Write();
                    } else {
                        Write(http::statuses::notFound);
                    }
                } else {
                    Write(http::statuses::badMethod);
                }
            } else if (m_request.method == "POST") {
                HandlePDF(match[1]);
            } else {
                Write(http::statuses::badMethod);
            }
        } else {
            Write(http::statuses::badMethod);
        }
    }
}

std::string Session::GetAboutReply()
{
    std::string content;

    content += "{";
    content += "\"status\": \"ok\", ";
    content += "\"version\": \"" + cefpdf::constants::version + "\", ";
    content += "\"persistent\": " + (m_sessionManager->m_persistent ? std::string("true") : std::string("false")) + ", ";
    content += "\"headers\": [";
    content += "\"" + http::headers::location + "\", ";
    content += "\"" + http::headers::pageSize + "\", ";
    content += "\"" + http::headers::pageMargin + "\", ";
    content += "\"" + http::headers::download + "\", ";
    content += "\"" + http::headers::pdfOptions + "(landscape|backgrounds)\"";
    content += "]}";

    return content;
}

std::string Session::GetPageSizesReply()
{
    std::string content;

    content += "[";
    cefpdf::PageSizesMap::const_iterator it;

    for (it = cefpdf::pageSizesMap.begin(); it != cefpdf::pageSizesMap.end(); ++it) {
        if (content.size() > 1) {
            content += ", ";
        }

        content += "\"" + it->name + "\"";
    }

    content += "]";

    return content;
}

void Session::HandlePDF(const std::string& fileName)
{
    m_response.SetHeader(http::headers::disposition, "inline; filename=\"" + fileName + "\"");

    std::string path;
    CefRefPtr<cefpdf::job::Job> job;

    if (fileName.length() + 1 == m_request.url.length()) {
        path = m_sessionManager->m_save;
    } else {
        path = m_sessionManager->m_save + m_request.url.substr(0, m_request.url.find_last_of("/"));
    }
    if (!std::filesystem::is_directory(std::filesystem::status(path))) {
        if (!std::filesystem::create_directories(path)) {
            m_response.SetContent("{\"status\":\"fail\",\"msg\":\"create directory fail\"}", "application/json");
            return;
        }
    }

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

    job->SetOutputPath(m_sessionManager->m_save + m_request.url);

    job->SetCallback(std::bind(
        &Session::OnResolve,
        this,
        std::placeholders::_1
    ));

    CefPostTask(TID_UI, base::BindOnce(&cefpdf::Client::AddJob, m_client, job));
}

void Session::OnResolve(CefRefPtr<cefpdf::job::Job> job)
{
    if (!m_socket.is_open()) {
        return;
    }

    if (job->GetStatus() == cefpdf::job::Job::Status::SUCCESS) {
        if (m_request.download) {
            m_response.SetContent(loadTempFile(job->GetOutputPath(), !m_sessionManager->m_persistent), "application/pdf");
        } else {
            m_response.SetContent("{\"status\":\"ok\",\"path\":\"" + m_request.url + "\"}", "application/json");
        }
    } else {
        m_response.SetStatus(http::statuses::error);
    }

    Write();
}

} // namespace server
} // namespace cefpdf
