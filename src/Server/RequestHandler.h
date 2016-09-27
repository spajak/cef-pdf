#ifndef SERVER_REQUEST_HANDLER_H_
#define SERVER_REQUEST_HANDLER_H_

#include "Http.h"

#include <string>

namespace cefpdf {
namespace server {

class RequestHandler
{
public:
    RequestHandler() {};
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    void handle(const http::Request& request, http::Response& response);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_REQUEST_HANDLER_H_