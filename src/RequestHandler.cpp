#include "StdInputSchemeHandlerFactory.h"

#include <string>
#include <iostream>
#include "include/wrapper/cef_stream_resource_handler.h"

StdInputSchemeHandlerFactory::StdInputSchemeHandlerFactory() {}



CefRefPtr<CefResourceHandler> StdInputSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request
) {
    CefRefPtr<CefStreamReader> stream = GetStdInputStream();
    return new CefStreamResourceHandler("text/html", stream);
}
