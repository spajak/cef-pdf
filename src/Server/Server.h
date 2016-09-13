#ifndef SERVER_H_
#define SERVER_H_

#include "include/cef_base.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <system_error>

#include <asio.hpp>
#include <asio/steady_timer.hpp>

namespace cefpdf {
namespace server {

class Server : public CefBase
{

public:
    Server() :
        m_thread(),
        m_ioService(),
        m_timer(m_ioService),
        counter(0),
        started(false) {};

    void Start()
    {
        if (!started) {
            m_thread = std::thread([&]{
                Listen();
                m_ioService.run();
            });
        }
    }

    void Stop()
    {
        if (started) {
            m_ioService.stop();
            m_thread.join();
            m_ioService.reset();
        }
    };

    void Listen()
    {
        m_timer.expires_from_now(std::chrono::seconds(1));
        m_timer.async_wait([&](std::error_code error) {
            std::cout << "Server fires: " << ++counter << std::endl;
            Listen();
        });
    }

private:
    bool started;
    std::thread m_thread;
    int counter;
    asio::io_service m_ioService;
    asio::steady_timer m_timer;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Server);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_H_
