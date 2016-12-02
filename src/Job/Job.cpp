#include "Job.h"

namespace cefpdf {
namespace job {

Job::Job()
{
    SetPageSize(cefpdf::constants::pageSize);
    SetPageMargin("default");
    m_pageOrientation = PageOrientation::PORTRAIT;
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

CefPdfPrintSettings Job::GetCefPdfPrintSettings() const
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

} // namespace job
} // namespace cefpdf
