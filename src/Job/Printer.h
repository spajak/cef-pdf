#ifndef JOB_PRINTER_H_
#define JOB_PRINTER_H_

#include "Manager.h"
#include "Visitor.h"
#include "Local.h"
#include "Remote.h"
#include "StdInput.h"

namespace cefpdf {
namespace job {

class Printer : public Visitor,
                public CefPdfPrintCallback
{
public:
    Printer(CefRefPtr<Manager> manager, CefRefPtr<CefBrowser> browser);

    void Print(CefRefPtr<Job> job);

    virtual void visit(CefRefPtr<Local> job);

    virtual void visit(CefRefPtr<Remote> job);

    virtual void visit(CefRefPtr<StdInput> job);

    // CefPdfPrintCallback methods:
    void OnPdfPrintFinished(const CefString& path, bool ok) override;

private:
    CefRefPtr<Manager> m_manager;
    CefRefPtr<CefBrowser> m_browser;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Printer);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_PRINTER_H_