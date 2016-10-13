#include "Server.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <chrono>
#include <system_error>
#include <iostream>
#include <utility>

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
    m_requestHandler(),
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
    m_thread = std::thread([&]{
        ListenSignals();
        ListenConnections();
        m_ioService.run();
        std::cout << "Thread finished" << std::endl;
    });

    m_client->Run();
    m_thread.join();
}

void Server::ListenConnections()
{
    //m_timer.expires_from_now(std::chrono::seconds(1));
    m_acceptor.async_accept(m_socket, [&](std::error_code error) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!m_acceptor.is_open()) {
            return;
        }

        CloseConnections(false);

        if (!error) {
            m_connections.insert(new Connection(std::move(m_socket), m_requestHandler));
            std::cout << "Server fires: " << ++m_counter << ", error: " << error << std::endl;
            ListenConnections();
        }
    });
}

void Server::ListenSignals()
{
    m_signals.async_wait([&](std::error_code error, int signno) {
        std::cout << "Server should quit" << std::endl;
        m_acceptor.close();
        CloseConnections();
        CefPostTask(TID_UI, base::Bind(&Client::Stop, m_client.get()));
    });
}

void Server::CloseConnections(bool force)
{
    for (auto it = m_connections.begin(); it != m_connections.end();) {
        if ((*it)->isOpen()) {
            if (force) {
                (*it)->Close();
            } else {
                ++it; continue;
            }
        }

        it = m_connections.erase(it);
    }
}

} // namespace server
} // namespace cefpdf
