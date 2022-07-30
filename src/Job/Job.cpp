#include "Job.h"

#include <podofo.h>

#include <filesystem>

namespace cefpdf {
namespace job {

Job::Job() :
    m_outputPath(),
    m_pageSize(),
    m_pageOrientation(PageOrientation::PORTRAIT),
    m_pageMargin(),
    m_backgrounds(false),
    m_status(Job::Status::PENDING),
    m_callback(),
    m_scale(100)
{
    SetPageSize(cefpdf::constants::pageSize);
    SetPageMargin("default");
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

void Job::SetScale(int scale)
{
    DLOG(INFO) << "Scale factor: " << scale;
    m_scale = scale;
}

void Job::SetCover(const std::string& path)
{
    m_cover = path;
}

void Job::SetAppendix(const std::string& path)
{
    m_appendix = path;
}

void Job::SetStatus(Status status) {
    m_status = status;

    if (Status::SUCCESS == status) {
        std::error_code code;
        std::string orgFile = m_outputPath.ToString() + ".org.pdf";
        if (!m_cover.empty() || !m_appendix.empty()) {
            std::filesystem::rename(m_outputPath.ToString(), orgFile, code);
        }
        if (0 == code.value()) {
            try {
                if (!m_cover.empty() && m_appendix.empty()) {
                    PoDoFo::PdfMemDocument cover( m_cover.c_str() );
                    PoDoFo::PdfMemDocument save( orgFile.c_str() );

                    cover.Append( save );

                    cover.Write( m_outputPath.c_str() );
                } else if (!m_cover.empty() && !m_appendix.empty()) {
                    PoDoFo::PdfMemDocument cover( m_cover.c_str() );
                    PoDoFo::PdfMemDocument save( orgFile.c_str() );
                    PoDoFo::PdfMemDocument appendix( m_appendix.c_str() );

                    cover.Append( save );
                    cover.Append( appendix );

                    cover.Write( m_outputPath.c_str() );
                } else if (m_cover.empty() && !m_appendix.empty()) {
                    PoDoFo::PdfMemDocument save( orgFile.c_str() );
                    PoDoFo::PdfMemDocument appendix( m_appendix.c_str() );

                    save.Append( appendix );

                    save.Write( m_outputPath.c_str() );
                }

                std::filesystem::remove(orgFile);
            } catch( PoDoFo::PdfError & e ) {
                throw e;
            }
        }
    }
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
