#include "Client.h"
#include "Server/Server.h"
#include "Job/Remote.h"
#include "Job/StdInput.h"

#include <string>
#include <list>
#include <iostream>

#if defined(OS_WIN)
#include <io.h>
#include <fcntl.h>
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
    std::cout << "  cef-pdf [options] --url=<url>|--file=<path> [output]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help -h        This help screen." << std::endl;
    std::cout << "  --url=<url>      URL to load, may be http, file, data, anything supported by Chromium." << std::endl;
    std::cout << "  --file=<path>    File path to load using file:// scheme. May be relative to current directory." << std::endl;
    std::cout << "  --stdin          Read content from standard input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)." << std::endl;
    std::cout << "  --size=<spec>    Size (format) of the paper: A3, B2.. or custom <width>x<height> in mm." << std::endl;
    std::cout << "                   " << cefpdf::constants::pageSize << " is the default." << std::endl;
    std::cout << "  --list-sizes     Show all defined page sizes." << std::endl;
    std::cout << "  --landscape      Wheather to print with a landscape page orientation." << std::endl;
    std::cout << "                   Default is portrait." << std::endl;
    std::cout << "  --margin=<spec>  Paper margins in mm (much like CSS margin but without units)" << std::endl;
    std::cout << "                   If omitted some default margin is applied." << std::endl;
    std::cout << "  --javascript     Enable JavaScript." << std::endl;
    std::cout << "  --backgrounds    Print with backgrounds. Default is without." << std::endl;
    std::cout << "  --remote-trigger Defer printing until page evaluates:";
    std::cout << "                   window.cefPdfQuery({request: \"trigger\", onSuccess: function () {}, onFailure: function () {}});";
    std::cout << "                   Will abort immediately if the page doesn't evaluate:";
    std::cout << "                   window.cefPdfQuery({request: \"register\", onSuccess: function () {}, onFailure: function () {}});";
    std::cout << "                   synchronously during load (e.g. in a script tag in the body).";
    std::cout << std::endl;
    std::cout << "Server options:" << std::endl;
    std::cout << "  --server         Start HTTP server" << std::endl;
    std::cout << "  --host=<host>    If starting server, specify ip address to bind to." << std::endl;
    std::cout << "                   Default is " << cefpdf::constants::serverHost << std::endl;
    std::cout << "  --port=<port>    Specify server port number. Default is " << cefpdf::constants::serverPort << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  PDF file name to create. Default is to write binary data to standard output." << std::endl;
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

int runJob(CefRefPtr<cefpdf::Client> app, CefRefPtr<CefCommandLine> commandLine)
{
    CefRefPtr<cefpdf::job::Job> job;
    bool readFromStdIn = false;
    bool writeToStdOut = false;

    try {
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
                    throw std::string("input file does not exist");
                }

                url = cefpdf::pathToUri(path);
            }

            if (url.empty()) {
                throw std::string("no input specified");
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

    } catch (std::string error) {
        std::cerr << "ERROR: " << error << std::endl;
        app->Shutdown();
        return 1;
    }

    if (readFromStdIn && !writeToStdOut) {
        std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;
    }

    app->SetStopAfterLastJob(true);
    app->AddJob(job);
    app->Run();

    if (writeToStdOut) {
#if defined(OS_WIN)
        // On Windows, in text mode, new line characters are translated to CRLF
        // So we need to switch to binary mode
        _setmode(_fileno(stdout), _O_BINARY);
#endif // OS_WIN
        std::cout << cefpdf::loadTempFile(job->GetOutputPath());
    }

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

    return 0;
}

int runServer(CefRefPtr<cefpdf::Client> app, CefRefPtr<CefCommandLine> commandLine)
{
    std::string port = cefpdf::constants::serverPort;
    if (commandLine->HasSwitch("port")) {
        port = commandLine->GetSwitchValue("port").ToString();
    }

    std::string host = cefpdf::constants::serverHost;
    if (commandLine->HasSwitch("host")) {
        host = commandLine->GetSwitchValue("host").ToString();
    }

    CefRefPtr<cefpdf::server::Server> server = new cefpdf::server::Server(app, host, port);

    server->Start();

    return 0;
}

int main(int argc, char* argv[])
{
    CefRefPtr<cefpdf::Client> app = new cefpdf::Client();

#if defined(OS_WIN)
    CefMainArgs mainArgs(::GetModuleHandle(NULL));
#else
    CefMainArgs mainArgs(argc, argv);
#endif // OS_WIN

#if !defined(OS_MACOSX)
    // Execute the sub-process logic, if any. This will either return immediately for the browser
    // process or block until the sub-process should exit.
    int exitCode = app->ExecuteSubProcess(mainArgs);
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

    bool remoteTrigger = commandLine->HasSwitch("remote-trigger") && !commandLine->HasSwitch("server");

    if (commandLine->HasSwitch("help") || commandLine->HasSwitch("h")) {
        printHelp(getExecutableName(commandLine));
        return 0;
    }

    if (commandLine->HasSwitch("list-sizes")) {
        printSizes();
        return 0;
    }

    bool javascript = commandLine->HasSwitch("javascript") || remoteTrigger;
    app->Initialize(mainArgs);
    app->SetDisableJavaScript(!javascript);

    app->SetRemoteTrigger(remoteTrigger);

    return commandLine->HasSwitch("server") ? runServer(app, commandLine) : runJob(app, commandLine);
}
