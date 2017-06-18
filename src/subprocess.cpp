#include "Client.h"

// This entry point is for Mac OS X only
int main(int argc, char* argv[])
{
    CefRefPtr<cefpdf::Client> app = new cefpdf::Client();
    CefMainArgs mainArgs(argc, argv);
    return app->ExecuteSubProcess(mainArgs);
}
