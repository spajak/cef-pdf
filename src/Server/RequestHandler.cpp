#include "RequestHandler.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <string>

namespace cefpdf {
namespace server {

void RequestHandler::Handle(const http::Request& request, http::Response& response)
{
    /*
    response.status = "HTTP/1.1 200 OK";
    response.headers.push_back({"Content-Type", "text/plain"});
    response.content = request.method + " " + request.url + " HTTP/" + request.version;
    */


/**
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503
*/


    std::string location;

    for (auto h: request.headers) {
        if (h.name.compare("Content-Location") == 0) {
            location = h.value;
            break;
        }
    }

    CefRefPtr<cefpdf::job::Job> job;

    if (location.empty()) {
        job = new cefpdf::job::Local(request.content);
    } else {
        job = new cefpdf::job::Remote(location);
    }

    auto future = job->GetFuture();

    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::PostJob, m_client.get(), job.get()));

    std::string result = future.get();

    if (result == "success") {
        response.status = "HTTP/1.1 200 OK";

        std::cout << job->GetOutputPath().ToString() << std::endl;

        auto file = cefpdf::file::load(job->GetOutputPath(), false);

        response.headers.push_back({"Content-Type", "application/pdf"});
        response.headers.push_back({"Content-Length", std::to_string(file.size())});

        response.content = file;
    } else {
        response.status = "HTTP/1.1 500 Internal Server Error";
    }
}

} // namespace server
} // namespace cefpdf
