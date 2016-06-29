#include "StringSchemeHandlerFactory.h"
#include "include/wrapper/cef_stream_resource_handler.h"

StringSchemeHandlerFactory::StringSchemeHandlerFactory() {}

StringSchemeHandlerFactory::StringSchemeHandlerFactory(const CefString& content)
{
    m_content = content;
}

void StringSchemeHandlerFactory::SetString(const CefString& content)
{
    m_content = content;
}

CefRefPtr<CefResourceHandler> StringSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    // Create a stream reader for body.
    CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForData(
        const_cast<char *>(m_content.ToString().c_str()),
        m_content.size()
    );

    CefResponse::HeaderMap headerMap;
    headerMap.insert(std::make_pair("Content-Type", "text/html; charset=utf-8"));

    return new CefStreamResourceHandler(200, "OK", "text/html", headerMap, stream);
}
