#ifndef STORAGE_H_
#define STORAGE_H_

#include "Job/Job.h"

#include "include/cef_base.h"

#include <string>

namespace cefpdf {

class Storage : public CefBase
{

public:
    Storage(std::string root) : m_root(root) {};

    // Generate file path from raw data
    std::string Reserve(std::string data);

    // Load content
    std::string Load(std::string path);

private:
    std::string m_root;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Storage);
};

} // namespace cefpdf

#endif // STORAGE_H_
