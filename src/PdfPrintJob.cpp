#include "PdfPrintJob.h"
#include "ResponseHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <cctype>
#include <fstream>
#include <string>
#include <algorithm>

namespace cefpdf {

PdfPrintJob::PdfPrintJob()
{
    m_url = constants::scheme + "://input";
    SetPageSize(constants::pageSize);
}

PdfPrintJob::PdfPrintJob(const CefString& url)
{
    m_url = url;
    SetPageSize(constants::pageSize);
}

const CefString& PdfPrintJob::GetUrl()
{
    return m_url;
}

void PdfPrintJob::SetUrl(const CefString& url)
{
    m_url = url;
}

const CefString& PdfPrintJob::GetContent()
{
    return m_content;
}

void PdfPrintJob::SetContent(const CefString& content)
{
    m_content = content;
}

const CefString& PdfPrintJob::GetOutputPath()
{
    return m_outputPath;
}

void PdfPrintJob::SetOutputPath(const CefString& outputPath)
{
    m_outputPath = outputPath;
}


void PdfPrintJob::SetPageSize(const CefString& pageSize)
{
    m_pageSize = parsePageSize(pageSize);
}

void PdfPrintJob::SetLandscape(bool flag)
{
    m_orientation = (flag ? PageOrientation::LANDSCAPE : PageOrientation::PORTRAIT);
}

void PdfPrintJob::SetPageMargin(const CefString& pageMargin)
{
    m_margin = parsePageMargin(pageMargin);
}

CefPdfPrintSettings PdfPrintJob::GetCefPdfPrintSettings()
{
    CefPdfPrintSettings pdfSettings;

    pdfSettings.backgrounds_enabled = true;
    pdfSettings.landscape = (m_orientation == PageOrientation::LANDSCAPE);

    auto pageSize = parsePageSize(m_pageSize);
    pdfSettings.page_width  = pageSize.width * 1000;
    pdfSettings.page_height = pageSize.height * 1000;

    if (!m_margin.empty()) {
        auto paperMargin = parseMargin(m_margin);
        // TODO: Set margins
    }

    return pdfSettings;
}

CefString PdfPrintJob::GetOutputContent()
{
    std::string content;
    if (!m_outputPath.empty()) {
        std::ifstream output(m_outputPath.ToString());
        content.assign((std::istreambuf_iterator<char>(output)), std::istreambuf_iterator<char>());
    }

    return content;
}

PageSize PdfPrintJob::parsePageSize(const CefString& paperSize)
{
    std::list<PageSize>::const_iterator it = paperSizesMap.find(paperSize);
    auto tolower = [](unsigned char c) { return std::tolower(c); };
    std::string lhs = paperSize.ToString();
    std::transform(lhs.begin(), lhs.end(), lhs.begin(), tolower);

    for (it = paperSizesMap.begin(); it != paperSizesMap.end(); ++it) {
        std::string rhs = it->name;
        std::transform(rhs.begin(), rhs.end(), rhs.begin(), tolower);

        if (lhs == rhs) {
            return *it;
        }
    }

    throw "Paper size \"" + paperSize.ToString() +"\" is not defined";
}

PageMargin PdfPrintJob::parsePageMargin(const CefString& margin)
{
    PageMargin pageMargin = {PDF_PRINT_MARGIN_CUSTOM, 0, 0, 0, 0};

    enum {
        PENDING = 0, TOP, RIGHT, BOTTOM, LEFT
    };

    typedef std::string::const_iterator Iter;
    int state = PENDING;
    std::string value;
    Iter begin = margin.ToString().begin();
    Iter end = margin.ToString().end();

    if (begin == end) {
        throw "Invalid margin format";
    }

    for (Iter i = begin; i != end; ++i) {
        char c = *i;
        if (0 != std::isdigit(c)) {
            value.push_back(c);
        } else if (0 != std::isspace(c)) {
            if (!value.empty()) {
                int v = std::atoi(value.c_str());
                switch (++state) {
                    case TOP:
                        pageMargin.top = pageMargin.right = pageMargin.bottom = pageMargin.left = v;
                        break;
                    case RIGHT:
                        pageMargin.right = pageMargin.left = v;
                        break;
                    case BOTTOM:
                        pageMargin.bottom = v;
                        break;
                    case LEFT:
                        pageMargin.left = v;
                        break;
                    default:
                        throw "Invalid margin format";
                }

                value.erase();
            }
        } else {
            throw "Invalid margin format";
        }
    }

    return pageMargin;
}

// CefRequestHandler methods:
// -----------------------------------------------------------------------------
CefRefPtr<CefResourceHandler> PdfPrintJob::GetResourceHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request
) {
    DLOG(INFO)
        << "GetResourceHandler"
        << ", request->url:" << request->GetURL().ToString();

    if (m_content.empty()) {
        // Allow the resource to load normally
        return NULL;
    }

    return new ResponseHandler(m_content);
}

} // namespace cefpdf
