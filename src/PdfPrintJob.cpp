#include "PdfPrintJob.h"
#include "ResponseHandler.h"

#include "include/wrapper/cef_helpers.h"

#include <cctype>
#include <fstream>
#include <string>

namespace cefpdf {

PdfPrintJob::PdfPrintJob()
{
    m_url = constants::scheme + "://input";
}

PdfPrintJob::PdfPrintJob(const CefString& url) : m_url(url) {}

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
    parsePageSize(pageSize);
    m_pageSize = pageSize;
}

void PdfPrintJob::SetLandscape(bool flag)
{
    m_orientation = (flag ? PageOrientation::LANDSCAPE : PageOrientation::PORTRAIT);
}

void PdfPrintJob::SetMargin(const CefString& margin)
{
    parseMargin(margin);
    m_margin = margin;
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
    PaperSizesMap::iterator it = paperSizesMap.find(paperSize);

    if (it == paperSizesMap.end()) {
        throw "Paper size \"" + paperSize.ToString() +"\" is not defined";
    }

    return it->second;
}

PageMargin PdfPrintJob::parsePageMargin(const CefString& margin)
{
    PageMargin pageMargin;

    enum {
        PENDING = 0, TOP, RIGHT, BOTTOM, LEFT
    };

    typedef std::string::const_iterator Iter;
    int state = PENDING;
    std::string value;
    Iter begin = margin.ToString().begin();
    Iter end = margin.ToString().end();

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
