#include "internal/cef_base.h";

class ContentProvider : public CefBase
{
    virtual ~ContentProvider();
    virtual const CefString& GetContent() = 0;
}
