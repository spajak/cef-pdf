#include "Server.h"
#include "RequestHandler.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <iostream>
#include <utility>
#include <functional>

using namespace std::placeholders;

namespace cefpdf {
namespace server {

Server::Server(
    CefRefPtr<cefpdf::Client> client,
    const std::string& address,
    const std::string& port
) :
    m_client(client),
    m_thread(),
    m_ioService(),
    m_signals(m_ioService),
    m_acceptor(m_ioService),
    m_socket(m_ioService),
    m_connectionManager(new ConnectionManager(new RequestHandler(client))),
    m_counter(0)
{
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
#if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(m_ioService);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();
}

void Server::Start()
{
    auto endpoint = m_acceptor.local_endpoint();

    std::cout << "Starting HTTP server on "
              << endpoint.address().to_string()
              << ":" << endpoint.port() << std::endl;

    m_thread = std::thread(std::bind(&Server::Run, this));

    m_client->SetAllowedSchemes({"http", "https", "ftp", "data"});
    m_client->Run();

    m_thread.join();
}

void Server::Run()
{
    m_signals.async_wait(std::bind(&Server::OnSignal, this, _1, _2));
    Listen();
    m_ioService.run();

    DLOG(INFO) << "Server thread finished";
}

void Server::Listen()
{
    m_acceptor.async_accept(m_socket, std::bind(&Server::OnConnection, this, _1));
}

void Server::OnSignal(std::error_code error, int signno)
{
    DLOG(INFO) << "Received shutdown signal";

    m_connectionManager->StopAll();
    m_acceptor.close();
    CefPostTask(TID_UI, base::Bind(&cefpdf::Client::Stop, m_client.get()));
}

void Server::OnConnection(std::error_code error)
{
    // Check whether the server was stopped by a signal before this
    // completion handler had a chance to run.
    if (!m_acceptor.is_open()) {
        return;
    }

    if (!error) {
        auto clientIp = m_socket.remote_endpoint().address().to_string();
        m_connectionManager->Start(
            new Connection(m_connectionManager, std::move(m_socket))
        );
        ++m_counter;
        LOG(INFO) << "Got hit no. " << m_counter << " from " << clientIp;
        Listen();
    }
}

} // namespace server
} // namespace cefpdf
