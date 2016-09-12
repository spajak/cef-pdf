#ifndef RESPONSE_HANDLER_H_
#define RESPONSE_HANDLER_H_

#include "Job/ContentProvider.h"

#include "include/cef_resource_handler.h"

#include <cstdlib>
#include <string>

namespace cefpdf {

class ResponseHandler : public CefResourceHandler
{
    public:

    ResponseHandler(CefRefPtr<job::ContentProvider> contentProvider);

    // CefResourceHandler methods:
    virtual bool ProcessRequest(
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback
    ) override;

    virtual void GetResponseHeaders(
        CefRefPtr<CefResponse> response,
        int64& response_length,
        CefString& redirectUrl
    ) override;

    virtual bool ReadResponse(
        void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefCallback> callback
    ) override;

    virtual void Cancel() override;

    private:

    CefRefPtr<job::ContentProvider> m_contentProvider;
    std::size_t m_offset;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ResponseHandler);
};

} // namespace cefpdf

#endif // RESPONSE_HANDLER_H_