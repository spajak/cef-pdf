#include "ResponseHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <string>
#include <iostream>

namespace cefpdf {

ResponseHandler::ResponseHandler(const CefString& data)
{
    m_data = data.ToString();
    m_offset = 0;
}

bool ResponseHandler::ProcessRequest(
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback
) {
    callback->Continue();
    return true;
}

void ResponseHandler::GetResponseHeaders(
    CefRefPtr<CefResponse> response,
    int64& response_length,
    CefString& redirectUrl
) {
    response->SetMimeType("text/html");
    response->SetStatus(200);

    // Set the resulting response length
    response_length = m_data.length();
}

bool ResponseHandler::ReadResponse(
    void* data_out,
    int bytes_to_read,
    int& bytes_read,
    CefRefPtr<CefCallback> callback
) {
    CEF_REQUIRE_IO_THREAD();

    bool has_data = false;
    bytes_read = 0;

    if (m_offset < m_data.length()) {
        // Copy the next block of data into the buffer.
        int transfer_size = std::min(bytes_to_read, static_cast<int>(m_data.length() - m_offset));
        std::memcpy(data_out, m_data.c_str() + m_offset, transfer_size);
        m_offset += transfer_size;

        bytes_read = transfer_size;
        has_data = true;
    }

    return has_data;
}

void ResponseHandler::Cancel() {}

CefRefPtr<CefResourceHandler> ResponseHandler::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    return this;
}

} // namespace cefpdf