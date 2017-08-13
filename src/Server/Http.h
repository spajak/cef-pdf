#ifndef SERVER_HTTP_H_
#define SERVER_HTTP_H_

#include <string>
#include <vector>
#include <ostream>

namespace cefpdf {
namespace server {
namespace http {

const std::string protocol = "HTTP/1.1";
const std::string crlf     = "\r\n";
const std::string hsep     = ": ";

namespace statuses {
    const std::string cont       = "100 Continue";
    const std::string ok         = "200 OK";
    const std::string badRequest = "400 Bad Request";
    const std::string notFound   = "404 Not Found";
    const std::string badMethod  = "405 Method Not Allowed";
    const std::string error      = "500 Internal Server Error";
} // namespace statuses

namespace headers {
    const std::string date        = "Date";
    const std::string type        = "Content-Type";
    const std::string length      = "Content-Length";
    const std::string disposition = "Content-Disposition";
    const std::string location    = "Content-Location";
    const std::string encoding    = "Transfer-Encoding";
    const std::string expect      = "Expect";
} // namespace headers

struct Header {
    std::string name;
    std::string value;
};

struct Request {
    std::string method;
    std::string url;
    std::string version;
    std::vector<Header> headers;
    std::string content;

    std::size_t contentLength;
    std::string transferEncoding;
    std::string expect;
    std::string location;
};

struct Response {
    std::string status;
    std::vector<Header> headers;
    std::string content;

    void SetStatus(const std::string& s) {
        status = protocol + " " + s;
    }

    void SetHeader(const std::string& header, const std::string& value) {
        headers.push_back({header, value});
    }

    void SetContent(const std::string& c, const std::string& t) {
        content = c;
        SetHeader(cefpdf::server::http::headers::type, t);
        SetHeader(cefpdf::server::http::headers::length, std::to_string(content.size()));
    }

    void WriteToStream(std::ostream& stream) {
        stream << status << crlf;
        for (auto const &header: headers) {
            stream << header.name << hsep << header.value << crlf;
        }

        stream << crlf << content;
    }
};

} // namespace http
} // namespace server
} // namespace cefpdf

#endif // SERVER_HTTP_H_
