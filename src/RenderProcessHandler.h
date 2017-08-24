#ifndef RENDER_PROCESS_HANDLER_H_
#define RENDER_PROCESS_HANDLER_H_

#include "include/cef_render_process_handler.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"

namespace cefpdf {

class RenderProcessHandler : public CefRenderProcessHandler,
                             public CefApp
{

public:
    RenderProcessHandler();

    // CefApp methods:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    // CefRenderProcessHandler methods:
    virtual void OnContextCreated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context
    ) override;
    virtual void OnContextReleased(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context
    ) override;
    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message
    ) override;
    virtual void OnWebKitInitialized() override;

private:
    CefRefPtr<CefMessageRouterRendererSide> m_messageRouterRendererSide;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(RenderProcessHandler)
};

} // namespace cefpdf

#endif // RENDER_PROCESS_HANDLER_H_
