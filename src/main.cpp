#include "Application.h"

int main(int argc, char* argv[])
{
    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    CefMainArgs mainArgs;
    int exitCode = CefExecuteProcess(mainArgs, NULL, NULL);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }

    // Implementation of the CefApp interface.
    CefRefPtr<Application> app(new Application);

    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
    commandLine->InitFromString(::GetCommandLineW());
#else
    commandLine->InitFromArgv(argc, argv);
#endif

    if (app->ParseCommandLine(commandLine)) {
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
    }

    return 0;
}
