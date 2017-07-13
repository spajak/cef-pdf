#include "RequestHandler.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"
#include "../Common.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <string>
#include <regex>

namespace cefpdf {
namespace server {

RequestHandler::RequestHandler(CefRefPtr<cefpdf::Client> client) :
    m_client(client) {}

void RequestHandler::Handle(const http::Request& request, http::Response& response)
{
    response.SetStatus(http::statuses::ok);
    response.SetHeader(http::headers::date, formatDate());

    if (!(request.method == "GET" || request.method == "POST")) {
        response.SetStatus(http::statuses::badMethod);
        return;
    }

    if (request.url == "/" || request.url == "/about") {
        response.SetContent(
            "{\"status\":\"ok\",\"version\":\"" + cefpdf::constants::version + "\"}",
            "application/json"
        );
        return;
    }

    // Parse url path
    std::regex re("([^/]+\\.pdf)($|[^\\w])", std::regex_constants::icase);
    std::smatch match;

    if (!std::regex_search(request.url, match, re)) {
        response.SetStatus(http::statuses::notFound);
        return;
    }

    std::string fileName = match[1];
    std::string location;

    for (auto h: request.headers) {
        if (h.name.compare(http::headers::location) == 0) {
            location = h.value;
            break;
        }
    }

    CefRefPtr<cefpdf::job::Job> job;

    if (location.empty()) {
        if (request.method != "POST") {
            response.SetStatus(http::statuses::badMethod);
            return;
        }

        if (request.content.empty()) {
            response.SetStatus(http::statuses::badRequest);
            return;
        }

        job = new cefpdf::job::Local(request.content);
    } else {
        job = new cefpdf::job::Remote(location);
    }

    auto future = job->GetFuture();
    //m_client->PostJob(job);
    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::AddJob, m_client, job.get()));
    std::string result = future.get();

    if (result == "success") {
        response.SetContent(loadTempFile(job->GetOutputPath()), "application/pdf");
        response.SetHeader(http::headers::disposition, "inline; filename=\"" + fileName + "\"");
    } else {
        response.SetStatus(http::statuses::error);
    }
}

} // namespace server
} // namespace cefpdf
