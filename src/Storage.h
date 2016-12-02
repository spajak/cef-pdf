#ifndef STORAGE_H_
#define STORAGE_H_

#include "Job/Job.h"

#include "include/cef_base.h"

#include <string>

namespace cefpdf {

class Storage : public CefBase
{

public:
    Storage();

    // Generate file path
    std::string Reserve();

    // Load content
    std::string Load(const std::string&, bool remove = true);

    // Delete file
    bool Delete(const std::string&);

    // Get system temp dir
    std::string GetTempPath();

private:
    std::string m_temp;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Storage);
};

} // namespace cefpdf

#endif // STORAGE_H_
