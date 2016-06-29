#include "PDFBrowserHandler.h"
#include "PDFRenderHandler.h"

#include "include\cef_app.h"
#include "include\wrapper\cef_helpers.h"

PDFBrowserHandler::PDFBrowserHandler() {}

// CefClient methods:
// -------------------------------------------------------------------------
CefRefPtr<CefLifeSpanHandler> PDFBrowserHandler::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> PDFBrowserHandler::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefRenderHandler> PDFBrowserHandler::GetRenderHandler()
{
    return new PDFRenderHandler;
}

// CefLifeSpanHandler methods:
// -------------------------------------------------------------------------
void PDFBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "PDFBrowserHandler::OnAfterCreated";

    CEF_REQUIRE_UI_THREAD();

    if (!m_browser.get()) {
        m_browser = browser;

        // set default string encoding to UTF-8
        CefRefPtr<CefValue> value = CefValue::Create();
        value->SetString("utf-8");

        CefString error;

        CefRequestContext::GetGlobalContext()
            ->SetPreference("intl.charset_default", value, error);
    }

    m_browserCount++;
}

bool PDFBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "PDFBrowserHandler::DoClose";

    CEF_REQUIRE_UI_THREAD();

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void PDFBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DLOG(INFO) << "PDFBrowserHandler::OnBeforeClose";

    CEF_REQUIRE_UI_THREAD();
    DCHECK(m_browser.get());

    if (browser->IsSame(m_browser)) {
        // Free the browser pointer so that the browser can be destroyed.
        m_browser = NULL;
    }

    if (--m_browserCount == 0) {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

// CefPdfPrintCallback methods:
// -------------------------------------------------------------------------
void PDFBrowserHandler::OnPdfPrintFinished(const CefString& path, bool ok)
{
    DLOG(INFO) << "PDFBrowserHandler::OnPdfPrintFinished";

    CEF_REQUIRE_UI_THREAD();

    m_browser->GetHost()->CloseBrowser(true);
}

// CefLoadHandler methods:
// -------------------------------------------------------------------------
void PDFBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    DLOG(INFO) << "PDFBrowserHandler::OnLoadEnd";

    CEF_REQUIRE_UI_THREAD();

    CefPdfPrintSettings pdfSettings;
    pdfSettings.backgrounds_enabled = true;

    m_browser->GetHost()->PrintToPDF("test.pdf", pdfSettings, this);
}

void PDFBrowserHandler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl
) {
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED) {
        return;
    }

    // TODO: Display a load error message.
}
