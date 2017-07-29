#include "SessionManager.h"

namespace cefpdf {
namespace server {

SessionManager::SessionManager()
{}

void SessionManager::Start(CefRefPtr<Session> session)
{
    session->Start();
    m_sessions.insert(session);
}

void SessionManager::Stop(CefRefPtr<Session> session)
{
    session->Close();
    m_sessions.erase(session);
}

void SessionManager::CloseAll()
{
    for (auto c: m_sessions) {
        c->Close();
    }
}

void SessionManager::StopAll()
{
    CloseAll();
    m_sessions.clear();
}

} // namespace server
} // namespace cefpdf
