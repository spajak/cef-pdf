#ifndef COMMON_H_
#define COMMON_H_

#include "include/internal/cef_types.h"

#include <string>
#include <list>
#include <chrono>
#include <ctime>

namespace cefpdf {

std::string getCurrentWorkingDirectory();

std::string getTempDirectory();

std::string getProcessId();

namespace constants {
    // cef-pdf version number
    const std::string version = "0.3.3";
    // Internal scheme
    const std::string scheme = "cefpdf";
    // Default page size
    const std::string pageSize = "A4";
    // Default character encoding
    const std::string encoding = "utf-8";
    // Max number of concurrent processes (browsers)
    const int maxProcesses = 10;
    // Default host name
    const std::string serverHost = "127.0.0.1";
    // Default TCP server port
    const std::string serverPort = "9288";

    // Current working directory
    const std::string cwd = getCurrentWorkingDirectory();
    // Temp directory (including trailing slash)
    const std::string tmp = getTempDirectory();
    // Current process ID
    const std::string pid = getProcessId();

    // JavaScript trigger functions
    const std::string jsQueryFunction = "cefPdfQuery";
    const std::string jsCancelFunction = "cefPdfCancel";

    // Used to calulate browser size and based on Chromium defaults
    const double micronPerPx = 264;
}

struct PageSize
{
    std::string name;
    int width, height;
};

typedef cef_pdf_print_margin_type_t PageMarginType;

struct PageMargin
{
    PageMarginType type;
    int top, right, bottom, left;
};

enum struct PageOrientation {
    PORTRAIT, LANDSCAPE
};

typedef std::list<PageSize> PageSizesMap;

extern PageSizesMap pageSizesMap;

PageSize getPageSize(const CefString& str);

PageMargin getPageMargin(const CefString& str);

std::string pathToUri(const std::string&);

bool fileExists(const std::string& path);

std::string reserveTempFile();

std::string loadTempFile(const std::string&, bool remove = true);

bool deleteTempFile(const std::string&);

std::string formatDate(const char* format = "%a, %d %h %Y %T GMT", std::time_t* arg = nullptr);

bool stringsEqual(const std::string&, const std::string&);

bool matchScheme(const std::string&, const std::string&);

} // namespace cefpdf

#endif // COMMON_H_
