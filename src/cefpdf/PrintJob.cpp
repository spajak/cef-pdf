// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "PrintJob.h"

namespace cefpdf {

PrintJob::PrintJob(const CefString& url) :
    m_pageSize(),
    m_pageOrientation(PageOrientation::PORTRAIT),
    m_pageMargin(),
    m_backgrounds(false),
    m_javascript(false),
    m_scale(100),
    m_delay(0),
    m_url(url),
    m_streamReader(NULL),
    m_outputPath(),
    m_status(PrintJob::Status::PENDING),
    m_error()
{
    SetPageSize(cefpdf::constants::pageSize);
    SetPageMargin("default");
}

void PrintJob::SetPageSize(const CefString& pageSize)
{
    m_pageSize = getPageSize(pageSize);
}

void PrintJob::SetLandscape(bool flag)
{
    m_pageOrientation = (flag ? PageOrientation::LANDSCAPE : PageOrientation::PORTRAIT);
}

void PrintJob::SetPageMargin(const CefString& pageMargin)
{
    m_pageMargin = getPageMargin(pageMargin);
}

void PrintJob::SetBackgrounds(bool flag)
{
    m_backgrounds = flag;
}

void PrintJob::SetJavaScript(bool flag)
{
    m_javascript = flag;
}

bool PrintJob::GetJavaScript()
{
    return m_javascript;
}

void PrintJob::SetScale(int scale)
{
    m_scale = scale;
}

void PrintJob::SetDelay(int delay)
{
    m_delay = delay;
}

int PrintJob::GetDelay()
{
    return m_delay;
}

const CefString& PrintJob::GetUrl() const
{
    return m_url;
}

CefRefPtr<CefStreamReader> PrintJob::GetStreamReader()
{
    return m_streamReader;
}

void PrintJob::SetStreamReader(CefRefPtr<CefStreamReader> reader)
{
    m_streamReader = reader;
}

const CefString& PrintJob::GetOutputPath() const
{
    return m_outputPath;
}

void PrintJob::SetOutputPath(const CefString& outputPath)
{
    m_outputPath = outputPath;
}

void PrintJob::SetError(const PrintJob::Error& error)
{
    m_error = error;
}

const PrintJob::Error& PrintJob::GetError()
{
    return m_error;
}

PrintJob::Status PrintJob::GetStatus()
{
    return m_status;
}

void PrintJob::SetStatus(PrintJob::Status status)
{
    m_status = status;
}

bool PrintJob::IsSuccess()
{
    return m_status == Status::SUCCESS;
}

bool PrintJob::IsFailed()
{
    return m_status == Status::FAILED;
}

bool PrintJob::IsFinished()
{
    return IsSuccess() || IsFailed();
}

/*
    virtual const std::string& GetContent() const {
        return m_content;
    }

    void SetContent(const std::string& content) {
        m_content = content;
    }
*/

CefPdfPrintSettings PrintJob::GetCefPdfPrintSettings() const
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

} // namespace cefpdf
