// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include <string>
#include <list>
#include <iostream>

#if defined(OS_WIN)
#include <io.h>
#include <fcntl.h>
#endif // OS_WIN

#include "cefpdf/App.h"
#include "cefpdf/job/Remote.h"
#include "cefpdf/job/StdInput.h"

#include "include/base/cef_logging.h"

void printHelp()
{
    std::cout << "cef-pdf v" << cefpdf::constants::version << std::endl;
    std::cout << "  Creates PDF files from HTML pages" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  cef-pdf [options] --url=<url>|--file=<path> [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h         This help screen." << std::endl;
    std::cout << "  --url=<url>       URL to load, may be http, file, data, anything supported by Chromium." << std::endl;
    std::cout << "  --file=<path>     File path to load using file:// scheme. May be relative to current directory." << std::endl;
    std::cout << "  --stdin           Read content from standard input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)." << std::endl;
    std::cout << "  --size=<spec>     Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm." << std::endl;
    std::cout << "                    " << cefpdf::constants::pageSize << " is the default." << std::endl;
    std::cout << "  --list-sizes      Show all defined page sizes." << std::endl;
    std::cout << "  --landscape       Wheather to print with a landscape page orientation." << std::endl;
    std::cout << "                    Default is portrait." << std::endl;
    std::cout << "  --margin=<spec>   Paper margins in mm (much like CSS margin but without units)" << std::endl;
    std::cout << "                    If omitted some default margin is applied." << std::endl;
    std::cout << "  --javascript      Enable JavaScript." << std::endl;
    std::cout << "  --backgrounds     Print with backgrounds. Default is without." << std::endl;
    std::cout << "  --scale=<%>       Scale the output. Default is 100." << std::endl;
    std::cout << "  --delay=<ms>      Wait after page load before creating PDF. Default is 0." << std::endl;
    std::cout << "  --viewwidth=<px>  Width of viewport. Default is 128." << std::endl;
    std::cout << "  --viewheight=<px> Height of viewport. Default is 128." << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  PDF file name to create. Default is to write binary data to standard output." << std::endl;
    std::cout << std::endl;
}

void printSizes()
{
    cefpdf::PageSizesMap::const_iterator it;
    for (it = cefpdf::pageSizesMap.begin(); it != cefpdf::pageSizesMap.end(); ++it) {
        std::cout << it->name <<  " " << it->width << "x" << it->height << std::endl;
    }
}

void setOptions(CefRefPtr<cefpdf::job::Job> job, CefRefPtr<CefCommandLine> commandLine)
{
    if (commandLine->HasSwitch("size")) {
        job->SetPageSize(commandLine->GetSwitchValue("size"));
    }

    if (commandLine->HasSwitch("margin")) {
        job->SetPageMargin(commandLine->GetSwitchValue("margin"));
    }

    if (commandLine->HasSwitch("landscape")) {
        job->SetLandscape();
    }

    if (commandLine->HasSwitch("backgrounds")) {
        job->SetBackgrounds();
    }

    if (commandLine->HasSwitch("scale")) {
        job->SetScale(cefpdf::strToInt(commandLine->GetSwitchValue("scale")));
    }

    if (commandLine->HasSwitch("delay")) {
        job->SetDelay(cefpdf::strToInt(commandLine->GetSwitchValue("delay")));
    }
/*
    if (commandLine->HasSwitch("viewwidth")) {
        app->SetViewWidth(std::atoi(commandLine->GetSwitchValue("viewwidth").ToString().c_str()));
    }

    if (commandLine->HasSwitch("viewheight")) {
        app->SetViewHeight(std::atoi(commandLine->GetSwitchValue("viewheight").ToString().c_str()));
    }
*/
    job->SetJavaScript(commandLine->HasSwitch("javascript"));
}

int runJob(CefRefPtr<cefpdf::App> app, CefRefPtr<CefCommandLine> commandLine)
{
    CefRefPtr<cefpdf::job::Job> job;
    bool readFromStdIn = false;
    bool writeToStdOut = false;

    if (commandLine->HasSwitch("stdin")) {
        job = new cefpdf::job::StdInput();
        readFromStdIn = true;
    } else {
        std::string url;

        if (commandLine->HasSwitch("url")) {
            url = commandLine->GetSwitchValue("url").ToString();
        } else if (commandLine->HasSwitch("file")) {
            auto path = commandLine->GetSwitchValue("file").ToString();

            if (!cefpdf::fileExists(path)) {
                std::cerr << "ERROR: " << "Input file does not exist" << std::endl;
                return 1;
            }

            url = cefpdf::pathToUri(path);
        }

        if (url.empty()) {
            std::cerr << "ERROR: " << "No input specified" << std::endl;
            return 1;
        }

        job = new cefpdf::job::Remote(url);
    }

    // Set output file
    CefCommandLine::ArgumentList args;
    commandLine->GetArguments(args);
    if (!args.empty()) {
        job->SetOutputPath(args[0]);
    } else {
        writeToStdOut = true;
    }

    setOptions(job, commandLine);

    if (readFromStdIn && !writeToStdOut) {
        std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;
    }

    app->Queue(job);
    app->Run();

    if (writeToStdOut) {
#if defined(OS_WIN)
        // On Windows, in text mode, new line characters are translated to CRLF
        // So we need to switch to binary mode
        //_setmode(_fileno(stdout), _O_BINARY);
#endif // OS_WIN
        std::cout << cefpdf::loadTempFile(job->GetOutputPath());
    }

/*
    switch (job->GetStatus()) {
        case cefpdf::job::Job::Status::SUCCESS:
            std::clog << "Printing document finished successfully" << std::endl;
            break;
        case cefpdf::job::Job::Status::PRINT_ERROR:
            std::clog << "Printing document failed!!" << std::endl;
            return 1;
        default:
            std::clog << "Loading document failed!!" << std::endl;
            return 1;
    }
*/
    return 0;
}

int main(int argc, char* argv[])
{
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();

#if defined(OS_WIN)
    CefMainArgs mainArgs(::GetModuleHandle(NULL));
    commandLine->InitFromString(::GetCommandLine());
    CefEnableHighDPISupport();
#else
    CefMainArgs mainArgs(argc, argv);
    commandLine->InitFromArgv(argc, argv);
#endif // OS_WIN

    CefRefPtr<cefpdf::App> app = new cefpdf::App(mainArgs);

    DLOG(INFO) << "Starting process \"" << app->GetProcessType(commandLine) << "\"";

#if !defined(OS_MACOSX)
    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int exitCode = CefExecuteProcess(mainArgs, app, NULL);
    if (exitCode >= 0) {
        // The sub-process terminated, exit now.
        return exitCode;
    }
#endif // !OS_MACOSX

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp();
        return 0;
    }

    if (commandLine->HasSwitch("list-sizes")) {
        printSizes();
        return 0;
    }

    return runJob(app, commandLine);
}
