#ifndef SIMPLE_CLIENT_H_
#define SIMPLE_CLIENT_H_

#include "Client.h"

namespace cefpdf {

class SimpleClient : public Client
{
    public:

    SimpleClient();

    void QueueJob(CefRefPtr<job::Job> job);

    virtual void OnContextInitialized() override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    private:

    std::queue<CefRefPtr<job::Job>> m_jobs;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleClient);
};

} // namespace cefpdf

#endif // SIMPLE_CLIENT_H_
