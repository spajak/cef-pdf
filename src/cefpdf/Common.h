// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_COMMON_H_
#define CEFPDF_COMMON_H_

#include <ctime>
#include <string>
#include <list>
#include <thread>
#include <chrono>

#include "include/internal/cef_types.h"

namespace cefpdf {

std::string getCurrentWorkingDirectory();

std::string getTempDirectory();

std::string getProcessId();

namespace constants {
    // cef-pdf version number
    const std::string version = "0.4.0";
    // Default page size
    const std::string pageSize = "A4";
    // Default character encoding
    const std::string encoding = "utf-8";
    // Internal scheme
    const std::string scheme = "cefpdf";
    // Internal url for file content
    const std::string stringUrl = scheme + "://string";
    // Internal url for standard input content
    const std::string stdinUrl = scheme + "://stdin";
    // Internal url for string content
    const std::string fileUrl = scheme + "://file";

    // Max number of concurrent processes (browsers)
    const std::size_t maxProcesses = 8;

    // Current working directory
    const std::string cwd = getCurrentWorkingDirectory();
    // Temp directory (including trailing slash)
    const std::string tmp = getTempDirectory();
    // Current process ID
    const std::string pid = getProcessId();

    // JavaScript functions
    const std::string jsQueryFunction = "cefPdfQuery";
    const std::string jsCancelFunction = "cefPdfCancel";
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

int strToInt(const std::string&);

std::chrono::microseconds::rep microtime();

void sleep(int ms);

} // namespace cefpdf

#endif // CEFPDF_COMMON_H_
