#include "RequestHandler.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <string>
#include <regex>
#include <ctime>

namespace cefpdf {
namespace server {

void RequestHandler::Handle(const http::Request& request, http::Response& response)
{
    SetDate(response);

    if (!(request.method == "GET" || request.method == "POST")) {
        response.status = "HTTP/1.1 405 Method Not Allowed";
        return;
    }

    if (request.url == "/" || request.url == "/about") {
        response.status = "HTTP/1.1 200 OK";
        response.headers.push_back({"Content-Type", "application/json"});
        response.content = "{\"status\":\"ok\",\"version\":\"" + cefpdf::constants::version + "\"}";
        SetContentLength(response);
        return;
    }

    // Parse url path
    std::regex re("([^/]+)\\.pdf");
    std::smatch match;

    if (!std::regex_search(request.url, match, re)) {
        response.status = "HTTP/1.1 404 Not Found";
        return;
    }

    std::string fileName = match[1];
    std::string location;

    for (auto h: request.headers) {
        if (h.name.compare("Content-Location") == 0) {
            location = h.value;
            break;
        }
    }

    CefRefPtr<cefpdf::job::Job> job;

    if (location.empty()) {
        if (request.method != "POST") {
            response.status = "HTTP/1.1 405 Method Not Allowed";
            return;
        }

        if (request.content.empty()) {
            response.status = "HTTP/1.1 400 Bad Request";
            return;
        }

        job = new cefpdf::job::Local(request.content);
    } else {
        job = new cefpdf::job::Remote(location);
    }

    auto future = job->GetFuture();

    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::PostJob, m_client.get(), job.get()));

    std::string result = future.get();

    if (result == "success") {
        response.status = "HTTP/1.1 200 OK";

        response.content = m_client->GetStorage()->Load(job->GetOutputPath());

        response.headers.push_back({"Content-Type", "application/pdf"});
        response.headers.push_back({"Content-Disposition", "inline; filename=\"" + fileName + ".pdf\""});

        SetContentLength(response);
    } else {
        response.status = "HTTP/1.1 500 Internal Server Error";
    }
}

void RequestHandler::SetDate(http::Response& response)
{
    std::time_t t = std::time(nullptr);
    char buff[100];
    strftime(buff, 100, "%a, %d %h %Y %T GMT", std::gmtime(&t));
    response.headers.push_back({"Date", std::string(buff)});
}

void RequestHandler::SetContentLength(http::Response& response)
{
    response.headers.push_back({"Content-Length", std::to_string(response.content.size())});
}

} // namespace server
} // namespace cefpdf
