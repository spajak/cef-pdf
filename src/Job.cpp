#include "Job.h"

#include "include/wrapper/cef_helpers.h"

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <fstream>
#include <string>

namespace cefpdf {

Job::Job()
{
    m_url = constants::scheme + "://input";
    SetPageSize(constants::pageSize);
    m_pageMargin.type = PDF_PRINT_MARGIN_DEFAULT;
}

Job::Job(const CefString& url)
{
    m_url = url;
    SetPageSize(constants::pageSize);
    m_pageMargin.type = PDF_PRINT_MARGIN_DEFAULT;
}

const CefString& Job::GetUrl()
{
    return m_url;
}

void Job::SetUrl(const CefString& url)
{
    m_url = url;
}

const CefString& Job::GetContent()
{
    return m_content;
}

void Job::SetContent(const CefString& content)
{
    m_content = content;
}

const CefString& Job::GetOutputPath()
{
    return m_outputPath;
}

void Job::SetOutputPath(const CefString& outputPath)
{
    m_outputPath = outputPath;
}


void Job::SetPageSize(const CefString& pageSize)
{
    std::string lhs = strtolower(pageSize.ToString());
    std::list<PageSize>::const_iterator it;

    for (it = pageSizesMap.begin(); it != pageSizesMap.end(); ++it) {
        std::string rhs = strtolower(it->name);
        if (lhs == rhs) {
            m_pageSize = *it;
            return;
        }
    }

    m_pageSize.name = "Custom";
    ParsePageSize(pageSize);
}

void Job::SetLandscape(bool flag)
{
    m_pageOrientation = (flag ? PageOrientation::LANDSCAPE : PageOrientation::PORTRAIT);
}

void Job::SetPageMargin(const CefString& pageMargin)
{
    std::string margin = strtolower(pageMargin.ToString());

    if ("default" == margin) {
        m_pageMargin.type = PDF_PRINT_MARGIN_DEFAULT;
    } else if ("none" == margin) {
        m_pageMargin.type = PDF_PRINT_MARGIN_NONE;
    } else if ("minimum" == margin) {
        m_pageMargin.type = PDF_PRINT_MARGIN_MINIMUM;
    } else { // Custom
        m_pageMargin.type = PDF_PRINT_MARGIN_CUSTOM;
        ParsePageMargin(pageMargin);
    }
}

CefPdfPrintSettings Job::GetCefPdfPrintSettings()
{
    CefPdfPrintSettings pdfSettings;

    pdfSettings.backgrounds_enabled = true;
    pdfSettings.landscape = (m_pageOrientation == PageOrientation::LANDSCAPE);

    pdfSettings.page_width  = m_pageSize.width * 1000;
    pdfSettings.page_height = m_pageSize.height * 1000;

    // TODO: Set margins

    return pdfSettings;
}

CefString Job::GetOutputContent()
{
    std::string content;
    if (!m_outputPath.empty()) {
        std::ifstream output(m_outputPath.ToString());
        content.assign((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());
    }

    return content;
}

void Job::ParsePageSize(const CefString& paperSize)
{
    // TODO: Parse custom page size
    throw "Paper size \"" + paperSize.ToString() +"\" is not defined";
}

void Job::ParsePageMargin(const CefString& pageMargin)
{
    m_pageMargin.type = PDF_PRINT_MARGIN_CUSTOM;

    std::string input = pageMargin.ToString() + "+";
    enum State { PENDING = 0, TOP, RIGHT, BOTTOM, LEFT };
    int state = State::PENDING;
    std::string value;
    bool hasValue = false;

    for (std::string::const_iterator i = input.begin(); i != input.end(); ++i) {
        char c = *i;
        if (0 != std::isdigit(c)) {
            value.push_back(c);
        } else if ('+' == c) {
            if (value.empty()) {
                continue;
            }

            int v = std::atoi(value.c_str());
            switch (++state) {
                case State::TOP:
                    m_pageMargin.top = m_pageMargin.right = m_pageMargin.bottom = m_pageMargin.left = v;
                    break;
                case State::RIGHT:
                    m_pageMargin.right = m_pageMargin.left = v;
                    break;
                case State::BOTTOM:
                    m_pageMargin.bottom = v;
                    break;
                case State::LEFT:
                    m_pageMargin.left = v;
                    break;
                default:
                    throw "Too many values in margin";
            }

            hasValue = true;
            value.erase();
        } else {
            throw "Invalid character \"" + std::string(&c) + "\" in margin";
        }
    }

    if (!hasValue) {
        throw "Invalid margin format";
    }
}

} // namespace cefpdf
