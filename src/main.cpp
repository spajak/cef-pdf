#include "PDFApp.h"
#include "include/internal/cef_win.h"

int main(int argc, char* argv[])
{
    // Structure for passing command-line arguments.
    // The definition of this structure is platform-specific.
    CefMainArgs mainArgs(GetModuleHandle(NULL));

    // Optional implementation of the CefApp interface.
    CefRefPtr<PDFApp> app(new PDFApp);

    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int exit_code = CefExecuteProcess(mainArgs, app.get(), NULL);
    if (exit_code >= 0) {
        // The sub-process terminated, exit now.
        return exit_code;
    }

    // Populate this structure to customize CEF behavior.
    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.command_line_args_disabled = true;

    // Initialize CEF in the main process.
    CefInitialize(mainArgs, settings, app.get(), NULL);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}
