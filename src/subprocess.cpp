// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "cefpdf/App.h"

// This entry point is for Mac OS X only
int main(int argc, char* argv[])
{
    CefRefPtr<cefpdf::App> app = new cefpdf::App();
    CefMainArgs mainArgs(argc, argv);
    return CefExecuteProcess(mainArgs, app, NULL);
}
