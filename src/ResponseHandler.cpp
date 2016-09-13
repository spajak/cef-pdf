#include "ResponseHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <iostream>
#include <algorithm>

namespace cefpdf {

ResponseHandler::ResponseHandler(CefRefPtr<job::ContentProvider> contentProvider) :
    m_contentProvider(contentProvider),
    m_offset(0)
{}

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
    response_length = m_contentProvider->GetContentLength();
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
    std::size_t length = m_contentProvider->GetContentLength();

    if (m_offset < length) {
        // Copy the next block of data into the buffer.
        int transfer_size = std::min(bytes_to_read, static_cast<int>(length - m_offset));
        std::memcpy(data_out, m_contentProvider->GetContentPtr() + m_offset, transfer_size);
        m_offset += transfer_size;

        bytes_read = transfer_size;
        has_data = true;
    }

    return has_data;
}

void ResponseHandler::Cancel() {}

} // namespace cefpdf