#ifndef SERVER_CONNECTION_H_
#define SERVER_CONNECTION_H_

#include "include/cef_base.h"

namespace cefpdf {
namespace server {

class Connection : public CefBase
{
public:
    Connection() {};
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void start();
    void stop();

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Connection);
};

} // namespace server
} // namespace cefpdf

#endif // SERVER_CONNECTION_H_