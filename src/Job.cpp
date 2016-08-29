#include "Job.h"

#include "include/wrapper/cef_helpers.h"

#include <fstream>
#include <string>

namespace cefpdf {

Job::Job()
{
    m_url = constants::scheme + "://input";
    SetPageSize(constants::pageSize);
    SetPageMargin("default");
}

Job::Job(const CefString& url)
{
    m_url = url;
    SetPageSize(constants::pageSize);
    SetPageMargin("default");
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
    m_pageSize = getPageSize(pageSize);
}

void Job::SetLandscape(bool flag)
{
    m_pageOrientation = (flag ? PageOrientation::LANDSCAPE : PageOrientation::PORTRAIT);
}

void Job::SetPageMargin(const CefString& pageMargin)
{
    m_pageMargin = getPageMargin(pageMargin);
}

CefPdfPrintSettings Job::GetCefPdfPrintSettings()
{
    CefPdfPrintSettings pdfSettings;

    pdfSettings.backgrounds_enabled = true;
    pdfSettings.landscape = (m_pageOrientation == PageOrientation::LANDSCAPE);

    pdfSettings.page_width  = m_pageSize.width * 1000;
    pdfSettings.page_height = m_pageSize.height * 1000;

    pdfSettings.margin_type = m_pageMargin.type;
    pdfSettings.margin_top = m_pageMargin.top;
    pdfSettings.margin_right = m_pageMargin.right;
    pdfSettings.margin_bottom = m_pageMargin.bottom;
    pdfSettings.margin_left = m_pageMargin.left;

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

} // namespace cefpdf
