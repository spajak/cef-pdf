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

    m_requestHandler->Handle(connection->GetRequest(), connection->GetResponse());
    connection->Write();
}

} // namespace server
} // namespace cefpdf
