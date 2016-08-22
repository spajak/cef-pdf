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

CefRefPtr<PdfPrintJob> getJobFromCommandLine(CefRefPtr<CefCommandLine> commandLine)
{
    CefRefPtr<PdfPrintJob> job;

    if (commandLine->HasSwitch("output")) {
        job->SetOutputPath(commandLine->GetSwitchValue("output"));
    } else {
        job->SetOutputPath('output.pdf');
    }

    if (commandLine->HasSwitch("url")) {
        job->SetUrl(commandLine->GetSwitchValue("paper-size"));
    } else {
        job->SetContent(getStdInput());
    }

    if (commandLine->HasSwitch("paper-size")) {
        job->SetPaperSize(commandLine->GetSwitchValue("paper-size"));
    }

    if (commandLine->HasSwitch("margin")) {
        job->SetMargin(commandLine->GetSwitchValue("margin"));
    }

    if (commandLine->HasSwitch("landscape")) {
        job->SetLandscape();
    }

    return job;
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

    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
    commandLine->InitFromArgv(argc, argv);

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(::GetProgramName(commandLine->GetProgram()).ToString());
        return 0;
    }

    try {
        CefRefPtr<PdfPrintJob> job = getJobFromCommandLine(commandLine);
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return 1;
    }

    // Implementation of the CefApp interface.
    CefRefPtr<Application> app = new Application;
    app->PostPrintJob(job);

    app->Run();

    return 0;
}
