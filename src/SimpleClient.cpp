#include "SimpleClient.h"

#include <iostream>
#include <queue>

namespace cefpdf {

SimpleClient::SimpleClient() :
    cefpdf::Client() {}

void SimpleClient::QueueJob(CefRefPtr<job::Job> job)
{
    m_jobs.push(job);
}

void SimpleClient::OnContextInitialized()
{
    Client::OnContextInitialized();

    // Start all pending jobs
    while (!m_jobs.empty()) {
        PostJob(m_jobs.front());
        m_jobs.pop();
    }
}

void SimpleClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    Client::OnBeforeClose(browser);

    // Quit if this was the last browser
    if (GetProcessCount() == 0) {
        Stop();
    }
}

} // namespace cefpdf
