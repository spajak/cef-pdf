// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_PRINT_JOB_CALLBACK_H_
#define CEFPDF_PRINT_JOB_CALLBACK_H_

#include "PrintJob.h"

#include "include/cef_base.h"

namespace cefpdf {

class PrintJobCallback : public CefBaseRefCounted
{

public:
    PrintJobCallback() {};

    virtual void OnPrintStarted(CefRefPtr<PrintJob>) {};

    virtual void OnPrintFinished(CefRefPtr<PrintJob>) = 0;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(PrintJobCallback);
};

} // namespace cefpdf

#endif // CEFPDF_PRINT_JOB_CALLBACK_H_
