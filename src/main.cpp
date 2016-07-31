#include "Application.h"

int main(int argc, char* argv[])
{
    // Implementation of the CefApp interface.
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
    commandLine->InitFromString(::GetCommandLineW());
#else
    commandLine->InitFromArgv(argc, argv);
#endif
    CefRefPtr<Application> app(new Application(commandLine));

    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    CefMainArgs mainArgs;
    int exit_code = CefExecuteProcess(mainArgs, app.get(), NULL);
    if (exit_code >= 0) {
        // The sub-process terminated, exit now.
        return exit_code;
    }

    // Populate this structure to customize CEF behavior.
    CefSettings settings;
    //settings.single_process = true;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = true;
    settings.command_line_args_disabled = false;

    // Initialize CEF in the main process.
    CefInitialize(mainArgs, settings, app.get(), NULL);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();

    return 0;
}
