#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include "include/cef_scheme.h"

class RequestHandler : public CefResourceHandler,
                        public CefSchemeHandlerFactory
{
    public:

    RequestHandler();



    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RequestHandler);
};

#endif // REQUEST_HANDLER_H_