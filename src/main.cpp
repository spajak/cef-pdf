#include "Application.h"

#include <string>
#include <iostream>

void printHelp(std::string name)
{
    std::cout << name << " v" << VERSION << std::endl;
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

CefString getStdInput()
{
    std::string input;

    std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;

    for (std::string line; std::getline(std::cin, line);) {
        input.append(line + "\n");
    }

    if (input.size() == 0) {
        input = "<!DOCTYPE html>\n";
    }

    return input;
}

void getCommandLine(CefRefPtr<CefCommandLine> commandLine, CefRefPtr<PdfPrintJob> job)
{




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
    CefRefPtr<Application> app = new Application;

    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
    commandLine->InitFromArgv(argc, argv);

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(::GetProgramName(commandLine->GetProgram().ToString()));
        return 0;
    }

    CefString url;

    if (commandLine->HasSwitch("url")) {
        job = new PdfPrintJob('stdin://get');
    } else {

    }

    CefRefPtr<PdfPrintJob> job = new PdfPrintJob(url);

    if (!commandLine->HasArguments()) {

    } else {
        std::vector<CefString> args;
        commandLine->GetArguments(args);
        job = new PdfPrintJob(args[0]);

        if (args.size() > 1) {
            // Custom output filename
            job->SetOutputPath(args[1]);
        }
    }

    try {
        if (commandLine->HasSwitch("paper-size")) {
            job->SetPaperSize(commandLine->GetSwitchValue("paper-size"));
        }

        if (commandLine->HasSwitch("landscape")) {
            job->SetLandscape();
        }
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return 1;
    }

    app->Run();

    return 0;
}
