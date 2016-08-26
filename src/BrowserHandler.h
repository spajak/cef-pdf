#ifndef BROWSER_HANDLER_H_
#define BROWSER_HANDLER_H_

#include "PdfPrintJob.h"

#include "include/cef_client.h"
#include "include/cef_browser.h"
#include "include/cef_scheme.h"

namespace cefpdf {

class BrowserHandler :
{
    public:

    BrowserHandler();




    private:


    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserHandler);
};

} // namespace cefpdf

#endif // BROWSER_HANDLER_H_
