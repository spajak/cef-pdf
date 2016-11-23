#include "RequestHandler.h"

namespace cefpdf {
namespace server {

void RequestHandler::Handle(const http::Request& request, http::Response& response)
{
    for (auto h: request.headers) {

    }

    response.status = "HTTP/1.1 200 OK";
    response.headers.push_back({"Content-Type", "text/plain"});
    response.content = request.method + " " + request.url + " HTTP/" + request.version;

/**
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503
*/

}

} // namespace server
} // namespace cefpdf
