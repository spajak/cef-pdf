// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "Job.h"

namespace cefpdf {
namespace job {

Job::Job() :
    m_outputPath(),
    m_pageSize(),
    m_pageOrientation(PageOrientation::PORTRAIT),
    m_pageMargin(),
    m_backgrounds(false),
    m_javascript(false),
    m_scale(100),
    m_delay(0),
    m_error()
{
    SetPageSize(cefpdf::constants::pageSize);
    SetPageMargin("default");
}

const CefString& Job::GetOutputPath() const
{
    return m_outputPath;
}

void Job::SetOutputPath(const CefString& outputPath)
{
    m_outputPath = outputPath;
}

void Job::SetError(const CefString& error)
{
    m_error = error;
}

bool Job::HasError()
{
    return m_error.empty();
}

const CefString& Job::GetError()
{
    return m_error;
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

void Job::SetBackgrounds(bool flag)
{
    m_backgrounds = flag;
}

void Job::SetJavaScript(bool flag)
{
    m_javascript = flag;
}

bool Job::GetJavaScript()
{
    return m_javascript;
}

void Job::SetScale(int scale)
{
    DLOG(INFO) << "Scale factor: " << scale;
    m_scale = scale;
}

void Job::SetDelay(int delay)
{
    DLOG(INFO) << "Delay: " << delay;
    m_delay = delay;
}

int Job::GetDelay()
{
    return m_delay;
}

CefPdfPrintSettings Job::GetCefPdfPrintSettings() const
{
    CefPdfPrintSettings pdfSettings;

    pdfSettings.scale_factor = m_scale;
    pdfSettings.backgrounds_enabled = m_backgrounds;
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
