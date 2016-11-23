#include "Server.h"

#include "../Job/Local.h"
#include "../Job/Remote.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

#include <chrono>
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
    m_thread = std::thread(std::bind(&Server::Run, this));
    m_client->Run();
    m_thread.join();
}

void Server::Run()
{
    m_signals.async_wait(std::bind(&Server::OnSignal, this, _1, _2));
    Listen();
    m_ioService.run();
    std::cout << "Thread finished" << std::endl;
}

void Server::Listen()
{
    m_acceptor.async_accept(m_socket, std::bind(&Server::OnConnection, this, _1));
}

void Server::OnSignal(std::error_code error, int signno)
{
    std::cout << "Server should quit" << std::endl;
    m_acceptor.close();
    CloseConnections();

    CefPostTask(TID_UI, base::Bind(&Client::Stop, m_client.get()));
}

void Server::OnConnection(std::error_code error)
{
    ++m_counter;

    // Check whether the server was stopped by a signal before this
    // completion handler had a chance to run.
    if (!m_acceptor.is_open()) {
        return;
    }

    CloseConnections(false);

    if (!error) {
        CefRefPtr<Connection> connection(new Connection(std::move(m_socket)));
        connection->Start(std::bind(&Server::OnRequest, this, _1, connection));

        //m_connections.insert(connection);

        Listen();
    }
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

void Server::OnRequest(http::Request request, CefRefPtr<Connection> connection)
{
    std::string location;

    for (auto h: request.headers) {
        if (h.name.compare("Content-Location") == 0) {
            location = h.value;
            break;
        }
    }

    CefRefPtr<cefpdf::job::Job> job;

    if (location.empty()) {
        job = new cefpdf::job::Local(request.content);
    } else {
        job = new cefpdf::job::Remote(location);
    }

    auto future = job->GetFuture();

    CefPostTask(TID_UI, base::Bind(&Client::PostJob, m_client.get(), job.get()));

    std::string result = future.get();

    http::Response response;

    std::cout << result << std::endl;

    if (result == "success") {
        response.status = "HTTP/1.1 200 OK";

        std::cout << job->GetOutputPath().ToString() << std::endl;

        auto file = cefpdf::file::load(job->GetOutputPath(), true);

        response.headers.push_back({"Content-Type", "application/pdf"});
        response.headers.push_back({"Content-Length", std::to_string(file.size())});

        response.content = file;
    } else {
        response.status = "HTTP/1.1 500 Internal Server Error";
    }

    connection->Write(response);
}

} // namespace server
} // namespace cefpdf
