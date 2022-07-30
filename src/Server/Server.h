#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include "../Client.h"
#include "SessionManager.h"

#include "include/cef_base.h"

#include <string>
#include <thread>
#include <system_error> // std::error_code

#include <asio.hpp>

namespace cefpdf {
namespace server {

class Server : public CefBaseRefCounted
{

public:
    Server(CefRefPtr<cefpdf::Client> client, std::string const& host, std::string const& port, std::string const& save, std::string const& temp, bool const& persistent);

    void Start();

private:
    void Run();

    void Listen();

    void OnSignal(std::error_code, int);

    void OnConnection(std::error_code);

    CefRefPtr<cefpdf::Client> m_client;

    std::thread m_thread;

    asio::io_service m_ioService;

    asio::signal_set m_signals;

    asio::ip::tcp::acceptor m_acceptor;

    asio::ip::tcp::socket m_socket;

    CefRefPtr<SessionManager> m_sessionManager;

    int m_counter;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Server);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SERVER_H_
