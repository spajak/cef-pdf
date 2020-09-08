#ifndef SERVER_SESSION_MANAGER_H_
#define SERVER_SESSION_MANAGER_H_

#include "Session.h"

#include "include/cef_base.h"

#include <set>

namespace cefpdf {
namespace server {

class SessionManager : public CefBaseRefCounted
{

public:
    SessionManager();

    SessionManager(const SessionManager&) = delete;

    SessionManager& operator=(const SessionManager&) = delete;

    void Start(CefRefPtr<Session>);

    void Stop(CefRefPtr<Session>);

    void CloseAll();

    void StopAll();

private:
    std::set<CefRefPtr<Session>> m_sessions;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SessionManager);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SESSION_MANAGER_H_
