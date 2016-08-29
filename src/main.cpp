#include "Client.h"

#include <string>
#include <iostream>

void printSizes()
{
    std::list<PageSize>::const_iterator it;

    for (it = pageSizesMap.begin(); it != pageSizesMap.end(); ++it) {
        std::cout << it->name <<  " " << it->width << "x" << it->height << std::endl;
    }
}

void printHelp(std::string name)
{
    std::cout << name << " v" << cefpdf::constants::version << std::endl;
    std::cout << "  Creates PDF files from HTML pages" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  cef-pdf [options] [input] [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h           This help screen." << std::endl;
    std::cout << "  --size=<size>       Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm." << std::endl;
    std::cout << "                      Without <size> list available sizes. A4 is the default." << std::endl;
    std::cout << "  --landscape         Wheather to print with a landscape page orientation." << std::endl;
    std::cout << "                      Default is portrait" << std::endl;
    std::cout << "  --margin=<margin>   Paper margins in mm (much like CSS margin but without units)" << std::endl;
    std::cout << "                      If omitted default margin is applied." << std::endl;
    std::cout << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << "  URL to load, may be http, file, data, anything supported by Chrome." << std::endl;
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

std::string getExecutableName(CefString path)
{
    std::string program = path.ToString();

    // Remove directory if present.
    // Do this before extension removal in case directory has a period character.
    const std::size_t s = program.find_last_of("\\/");
    if (std::string::npos != s) {
        program.erase(0, s + 1);
    }

    // Remove extension if present.
    const std::size_t e = program.rfind('.');
    if (std::string::npos != e) {
        program.erase(e);
    }

    return program;
}

int main(int argc, char* argv[])
{
    CefRefPtr<cefpdf::Client> app = new cefpdf::Client(true);

    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    CefMainArgs mainArgs;
    int exitCode = CefExecuteProcess(mainArgs, app, NULL);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }

    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();

#if defined(OS_WIN)
    commandLine->InitFromString(::GetCommandLineW());
#else
    commandLine->InitFromArgv(argc, argv);
#endif

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(::getExecutableName(commandLine->GetProgram()));
        return 0;
    }

    CefRefPtr<cefpdf::Job> job = new cefpdf::Job;

    try {
        if (commandLine->HasSwitch("output")) {
            job->SetOutputPath(commandLine->GetSwitchValue("output"));
        } else {
            job->SetOutputPath("output.pdf");
        }

        if (commandLine->HasSwitch("url")) {
            job->SetUrl(commandLine->GetSwitchValue("url"));
        } else {
            job->SetContent(getStdInput());
        }

        if (commandLine->HasSwitch("size")) {
            job->SetPageSize(commandLine->GetSwitchValue("size"));
        }

        if (commandLine->HasSwitch("margin")) {
            job->SetPageMargin(commandLine->GetSwitchValue("margin"));
        }

        if (commandLine->HasSwitch("landscape")) {
            job->SetLandscape();
        }
    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        return 1;
    }

    app->QueueJob(job);
    app->Run();

    return 0;
}
