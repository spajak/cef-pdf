#ifndef SERVER_HTTP_H_
#define SERVER_HTTP_H_

#include <string>
#include <vector>

namespace cefpdf {
namespace server {
namespace http {

const std::string crlf = "\r\n";
const std::string hsep = ": ";

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
};

struct Response {
    std::string status;
    std::vector<Header> headers;
    std::string content;
};

} // namespace http
} // namespace server
} // namespace cefpdf

#endif // SERVER_HTTP_H_