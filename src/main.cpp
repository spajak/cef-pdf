#include "Application.h"

#include <iostream>

void printHelp(CefString name)
{
    std::cout << name.ToString() << " v" << VERSION << std::endl;
    std::cout << "  Creates PDF files from HTML pages" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  cef-pdf [options] [input] [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h            This help screen." << std::endl;
    std::cout << "  --paper-size=<size>  Size (format) of the paper: A3, B2.. Default is A4." << std::endl;
    std::cout << "  --landscape          Wheather to print with a landscape page orientation. Default is portrait" << std::endl;
    std::cout << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << "  URL to load, may be http, file, data, anything supported by Chrome." << std::endl;
    std::cout << "  Special url stdin:// scheme is used for inputing html from standard input. Default is stdin://get" << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  PDF file name. Standard output is not supported. Defaults to output.pdf" << std::endl;
    std::cout << std::endl;
}

bool parseCommandLine(CefRefPtr<CefCommandLine> commandLine)
{
    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(::GetProgramName(commandLine->GetProgram()));
        return false;
    }

    m_pdfSettings.backgrounds_enabled = true;
    m_pdfSettings.landscape = false;

    try {
        if (commandLine->HasSwitch("paper-size")) {
            m_paperSize = commandLine->GetSwitchValue("paper-size");
            if (m_paperSize.empty()) {
                throw "Paper size not specified";
            }
        }

        PaperSizes::iterator it = paperSizes.find(m_paperSize);
        if (it != paperSizes.end()) {
            m_pdfSettings.page_width = it->second.first * 1000;
            m_pdfSettings.page_height = it->second.second * 1000;
        } else {
            throw "Paper size \"" + m_paperSize.ToString() +"\" is not defined";
        }
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return false;
    }

    if (commandLine->HasSwitch("landscape")) {
        m_pdfSettings.landscape = true;
    }

    if (commandLine->HasArguments()) {
        std::vector<CefString> args;
        commandLine->GetArguments(args);
        if (args.size() > 0) {
            // Get input url
            m_urlInput = args[0];
            if (args.size() > 1) {
                // Get output filename
                m_pdfOutput = args[1];
            }
        }
    }

    return true;
}

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
