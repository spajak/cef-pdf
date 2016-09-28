#ifndef SERVER_HTTP_H_
#define SERVER_HTTP_H_

#include <string>
#include <vector>

namespace cefpdf {
namespace server {
namespace http {

struct Header {
    std::string name;
    std::string value;
};

enum struct Status {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503
};

enum struct Method {
    GET, POST
};

struct Version {
    int major, minor;
};

struct Request {
    Method method;
    std::string uri;
    Version version;
    std::vector<Header> headers;
    std::string content;
};

struct Response {
    Status status;
    std::vector<Header> headers;
    std::string content;
};

} // namespace http
} // namespace server
} // namespace cefpdf

#endif // SERVER_HTTP_H_