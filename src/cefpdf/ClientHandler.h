// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_CLIENT_HANDLER_H_
#define CEFPDF_CLIENT_HANDLER_H_

#include "Client.h"
#include "job/Job.h"

#include "include/cef_base.h"

namespace cefpdf {

class ClientHandler : public CefBaseRefCounted
{

public:
    ClientHandler() {};

    virtual void OnStarted(CefRefPtr<job::Job>) {};

    virtual void OnFinished(CefRefPtr<job::Job>, Client::Status) = 0;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ClientHandler);
};

} // namespace cefpdf

#endif // CEFPDF_CLIENT_HANDLER_H_
