#include "ConnectionManager.h"

#include <iostream>

namespace cefpdf {
namespace server {

void ConnectionManager::Start(CefRefPtr<Connection> connection)
{
    connection->Start();
    m_connections.insert(connection);
}

void ConnectionManager::Stop(CefRefPtr<Connection> connection)
{
    connection->Close();
    m_connections.erase(connection);
}

void ConnectionManager::StopAll()
{
    for (auto c: m_connections) {
        c->Close();
    }

    m_connections.clear();
}

void ConnectionManager::OnRequest(CefRefPtr<Connection> connection)
{
    auto r = connection->GetRequest();

    std::cout << "REQUEST ============================" << std::endl;
    std::cout << r.method << " " << r.url << " " << r.version << std::endl;
    for (auto h: r.headers) {
        std::cout << h.name << ": " << h.value << std::endl;
    }

    std::cout << r.content << std::endl;
    std::cout << "====================================" << std::endl;

    m_requestHandler->Handle(connection->GetRequest(), connection->GetResponse());
    connection->Write();
}

} // namespace server
} // namespace cefpdf
