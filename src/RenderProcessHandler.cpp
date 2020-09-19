#include "RenderProcessHandler.h"
#include "Common.h"

namespace cefpdf {

RenderProcessHandler::RenderProcessHandler() {}

// CefRenderProcessHandler methods:
// -----------------------------------------------------------------------------
void RenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "RenderProcessHandler::OnContextCreated";
    CEF_REQUIRE_RENDERER_THREAD();

    //m_messageRouterRendererSide->OnContextCreated(browser, frame, context);
}

void RenderProcessHandler::OnContextReleased(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context
) {
    DLOG(INFO) << "RenderProcessHandler::OnContextReleased";
    CEF_REQUIRE_RENDERER_THREAD();

    //m_messageRouterRendererSide->OnContextReleased(browser, frame, context);
}

bool RenderProcessHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message
) {
    DLOG(INFO) << "RenderProcessHandler::OnProcessMessageReceived";
    CEF_REQUIRE_RENDERER_THREAD();

    //m_messageRouterRendererSide->OnProcessMessageReceived(browser, source_process, message);
    return true;
}

void RenderProcessHandler::OnWebKitInitialized()
{
    DLOG(INFO) << "RenderProcessHandler::OnWebKitInitialized";
    CEF_REQUIRE_RENDERER_THREAD();

    // CefMessageRouterConfig config;
    // config.js_query_function = constants::jsQueryFunction;
    // config.js_cancel_function = constants::jsCancelFunction;

    // m_messageRouterRendererSide = CefMessageRouterRendererSide::Create(config);
}

} // namespace cefpdf

