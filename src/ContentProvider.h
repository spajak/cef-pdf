#include "internal/cef_string.h";

class ContentProvider
{
    virtual ~ContentProvider();
    virtual const CefString& GetContent() = 0;
}
