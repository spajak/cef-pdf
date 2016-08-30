#include "Job.h"

#include "include/wrapper/cef_helpers.h"

#include <string>

namespace cefpdf {

int Job::counter = 0;

Job::Job()
{
    Init();
    m_url = constants::scheme + "://job-" + std::to_string(GetID());
}

Job::Job(const CefString& url)
{
    Init();
    m_url = url;
}

void Job::Init()
{
    m_id = ++counter;

    m_status = Status::Pending;
    m_errorCode = ErrorCode::ERR_NONE;

    SetPageSize(constants::pageSize);
    SetPageMargin("default");
    m_outputPath = getTempPath() + "/cefpdf_" + std::to_string(GetID()) + ".pdf";
}

int Job::GetID() const
{
    return m_id;
}

const CefString& Job::GetUrl() const
{
    return m_url;
}

void Job::SetUrl(const CefString& url)
{
    m_url = url;
}

const CefString& Job::GetContent()
{
    if (m_content.empty()) {
        if (nullptr != m_contentProvider) {
            m_content = m_contentProvider();
        }
    }

    return m_content;
}

void Job::SetContent(const CefString& content)
{
    m_content = content;
}

void Job::SetContentProvider(ContentProvider const& provider)
{
    m_contentProvider = provider;
}

const CefString& Job::GetOutputPath() const
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

Job::Status Job::GetStatus() const
{
    return m_status;
}

Job::ErrorCode Job::GetStatusError() const
{
    return m_errorCode;
}

void Job::OnStatus(StatusCallback const& callback)
{
    m_statusCallback = callback;
}

void Job::SetStatus(Status status, ErrorCode errorCode)
{
    DCHECK(status >= m_status);
    m_errorCode = errorCode;

    if (status != m_status) {
        m_status = status;
        if (nullptr != m_statusCallback) {
            m_statusCallback(m_status, m_errorCode);
        }
    }
}

void Job::SetStatus(Status status)
{
    SetStatus(status, ErrorCode::ERR_NONE);
}

} // namespace cefpdf
