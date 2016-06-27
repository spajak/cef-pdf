#include "StringSchemeHandlerFactory.h"
#include "include/wrapper/cef_stream_resource_handler.h"

StringSchemeHandlerFactory::StringSchemeHandlerFactory(const CefString& content)
{
    m_content = content;
};

CefRefPtr<CefResourceHandler> StringSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    // Create a stream reader for body.
    CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(m_content)),
        body.size()
    );

    return new CefStreamResourceHandler("text/html", stream);
};
