#include "SimpleClient.h"
#include "Server/Client.h"
#include "Job/Remote.h"
#include "Job/StdInput.h"

#include <string>
#include <list>
#include <iostream>

#if defined(OS_WIN)
#pragma comment(linker, "/entry:mainCRTStartup /subsystem:console")
#endif // OS_WIN

void printSizes()
{
    cefpdf::PageSizesMap::const_iterator it;

    for (it = cefpdf::pageSizesMap.begin(); it != cefpdf::pageSizesMap.end(); ++it) {
        std::cout << it->name <<  " " << it->width << "x" << it->height << std::endl;
    }
}

void printHelp(std::string name)
{
    std::cout << name << " v" << cefpdf::constants::version << std::endl;
    std::cout << "  Creates PDF files from HTML pages" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  cef-pdf [options] [--url=<input>] [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h          This help screen." << std::endl;
    std::cout << "  --url=<input>      URL to load, may be http, file, data, anything supported by Chromium." << std::endl;
    std::cout << "                     If omitted standard input is read." << std::endl;
    std::cout << "  --size=<size>      Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm." << std::endl;
    std::cout << "                     A4 is the default." << std::endl;
    std::cout << "  --list-sizes       Show all defined page sizes." << std::endl;
    std::cout << "  --landscape        Wheather to print with a landscape page orientation." << std::endl;
    std::cout << "                     Default is portrait" << std::endl;
    std::cout << "  --margin=<margin>  Paper margins in mm (much like CSS margin but without units)" << std::endl;
    std::cout << "                     If omitted default margin is applied." << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  PDF file name to create. Default is output.pdf" << std::endl;
    std::cout << std::endl;
}

std::string getExecutableName(CefRefPtr<CefCommandLine> commandLine)
{
    std::string program = commandLine->GetProgram().ToString();

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

int runJob(CefRefPtr<CefCommandLine> commandLine)
{
    cefpdf::job::Job* job;

    if (commandLine->HasSwitch("url")) {
        job = new cefpdf::job::Remote(commandLine->GetSwitchValue("url"));
    } else {
        job = new cefpdf::job::StdInput;
    }

    // Set output file
    CefCommandLine::ArgumentList args;
    commandLine->GetArguments(args);
    if (!args.empty()) {
        job->SetOutputPath(args[0]);
    } else {
        job->SetOutputPath("output.pdf");
    }

    try {
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

    auto app = new cefpdf::SimpleClient();
    app->QueueJob(job);
    app->Run();

    return 0;
}

int main(int argc, char* argv[])
{

#if !defined(OS_MACOSX)
#if defined(OS_WIN)
    CefMainArgs mainArgs(::GetModuleHandle(NULL));
#else
    CefMainArgs mainArgs(argc, argv);
#endif // OS_WIN
    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int exitCode = CefExecuteProcess(mainArgs, NULL, NULL);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }
#endif // !OS_MACOSX

    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();

#if defined(OS_WIN)
    commandLine->InitFromString(::GetCommandLine());
#else
    commandLine->InitFromArgv(argc, argv);
#endif // OS_WIN

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(getExecutableName(commandLine));
        return 0;
    }

    if (commandLine->HasSwitch("list-sizes")) {
        printSizes();
        return 0;
    }

    if (commandLine->HasSwitch("server")) {
        CefRefPtr<cefpdf::server::Client> app = new cefpdf::server::Client();
        app->Run();
        return 0;
    }

    return runJob(commandLine);
}
