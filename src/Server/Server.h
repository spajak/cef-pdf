#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include "../Client.h"
#include "Connection.h"
#include "RequestHandler.h"

#include "include/cef_base.h"

#include <string>
#include <set>
#include <thread>

#include <asio.hpp>

namespace cefpdf {
namespace server {

class Server : public CefBase
{

public:
    Server(CefRefPtr<cefpdf::Client> client, std::string const&, std::string const&);
    void Start();

private:
    void ListenConnections();
    void ListenSignals();
    void CloseConnections(bool force = true);

    CefRefPtr<cefpdf::Client> m_client;
    std::thread m_thread;
    asio::io_service m_ioService;
    asio::signal_set m_signals;
    asio::ip::tcp::acceptor m_acceptor;
    asio::ip::tcp::socket m_socket;
    CefRefPtr<RequestHandler> m_requestHandler;
    int m_counter;

    std::set<CefRefPtr<Connection>> m_connections;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Server);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SERVER_H_
