#ifndef RESPONSE_HANDLER_H_
#define RESPONSE_HANDLER_H_

#include "include/cef_scheme.h"

class ResponseHandler : public CefResourceHandler,
                        public CefSchemeHandlerFactory
{
    public:

    ResponseHandler(CefRefPtr<PdfPrintJob> job);

    void Register();

    ///
    // Begin processing the request. To handle the request return true and call
    // CefCallback::Continue() once the response header information is available
    // (CefCallback::Continue() can also be called from inside this method if
    // header information is available immediately). To cancel the request return
    // false.
    ///
    virtual bool ProcessRequest(
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback
    ) OVERRIDE;

    ///
    // Retrieve response header information. If the response length is not known
    // set |response_length| to -1 and ReadResponse() will be called until it
    // returns false. If the response length is known set |response_length|
    // to a positive value and ReadResponse() will be called until it returns
    // false or the specified number of bytes have been read. Use the |response|
    // object to set the mime type, http status code and other optional header
    // values. To redirect the request to a new URL set |redirectUrl| to the new
    // URL. If an error occured while setting up the request you can call
    // SetError() on |response| to indicate the error condition.
    ///
    virtual void GetResponseHeaders(
        CefRefPtr<CefResponse> response,
        int64& response_length,
        CefString& redirectUrl
    ) OVERRIDE;

    ///
    // Read response data. If data is available immediately copy up to
    // |bytes_to_read| bytes into |data_out|, set |bytes_read| to the number of
    // bytes copied, and return true. To read the data at a later time set
    // |bytes_read| to 0, return true and call CefCallback::Continue() when the
    // data is available. To indicate response completion return false.
    ///
    virtual bool ReadResponse(
        void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefCallback> callback
    ) OVERRIDE;


    virtual CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request
    ) OVERRIDE;

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ResponseHandler);
};

#endif // RESPONSE_HANDLER_H_