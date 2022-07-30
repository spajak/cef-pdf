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
    SessionManager(std::string const& save, std::string const& temp, bool const& persistent);

    SessionManager(const SessionManager&) = delete;

    SessionManager& operator=(const SessionManager&) = delete;

    void Start(CefRefPtr<Session>);

    void Stop(CefRefPtr<Session>);

    void CloseAll();

    void StopAll();

    std::string m_save;

    std::string m_temp;

    bool m_persistent;

private:
    std::set<CefRefPtr<Session>> m_sessions;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SessionManager);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_SESSION_MANAGER_H_
