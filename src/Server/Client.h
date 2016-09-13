#ifndef SERVER_CLIENT_H_
#define SERVER_CLIENT_H_

#include "../Client.h"
#include "Server.h"

namespace cefpdf {
namespace server {

class Client : public cefpdf::Client
{

public:
    Client();

    virtual void Run() override;

private:
    CefRefPtr<Server> m_server;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CLIENT_H_
