#ifndef SERVER_H_
#define SERVER_H_

#include "include/cef_base.h"

#include <csignal>
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
        m_signals(m_ioService),
        m_timer(m_ioService),
        counter(0),
        started(false)
    {
          m_signals.add(SIGINT);
          m_signals.add(SIGTERM);
        #if defined(SIGQUIT)
          m_signals.add(SIGQUIT);
        #endif // defined(SIGQUIT)
    };

    void Start()
    {
        if (!started) {

            m_thread = std::thread([&]{
                ListenSignals();
                Listen();
                m_ioService.run();
            });

            started = true;
        }
    };

    void Stop()
    {
        if (started) {
            m_ioService.stop();
            m_thread.join();
            m_ioService.reset();
            started = false;
        }
    };

    void Listen()
    {
        m_timer.expires_from_now(std::chrono::seconds(1));
        m_timer.async_wait([&](std::error_code error) {
            std::cout << "Server fires: " << ++counter << ", error: " << error << std::endl;
            Listen();
        });
    };

    void ListenSignals()
    {
        m_signals.async_wait([&](std::error_code error, int signno) {
            std::cout << "Server should quit" << std::endl;
            m_timer.cancel();
        });
    };

private:
    std::thread m_thread;
    asio::io_service m_ioService;
    asio::signal_set m_signals;
    asio::steady_timer m_timer;
    int counter;
    bool started;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Server);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_H_
