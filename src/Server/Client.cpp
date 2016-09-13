#include "Client.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace cefpdf {
namespace server {

Client::Client() :
    cefpdf::Client()
{
    m_server = new Server;
}


void Client::Run()
{
    Initialize();

    m_server->Start();
    CefRunMessageLoop();
    m_server->Stop();

    CefShutdown();
}

} // namespace server
} // namespace cefpdf
