#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include "Job/Job.h"

#include "include/cef_base.h"

#include <string>
#include <utility>
#include <list>
#include <functional>

namespace cefpdf {

class EventManager : public CefBase
{

public:
    typedef std::function<void(CefRefPtr<job::Job>)> Callback;

    EventManager() {};

    void AddListener(const std::string& name, Callback callback) {
        m_callbacks.push_back( {name, callback} );
    };

    void Trigger(const std::string& name) {
        Trigger(name, NULL);
    };

    void Trigger(const std::string& name, CefRefPtr<job::Job> job) {
        for (auto i: m_callbacks) {
            if (0 == name.compare(i.first)) {
                (i.second)(job.get());
            }
        }
    };

private:
    std::list<std::pair<const std::string, Callback>> m_callbacks;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(EventManager);
};

} // namespace cefpdf

#endif // EVENT_MANAGER_H_
