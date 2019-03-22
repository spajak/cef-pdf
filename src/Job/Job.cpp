#include "Job.h"

namespace cefpdf
{
	namespace job
	{

		Job::Job() :
			m_outputPath(),
			m_pageSize(),
			m_pageOrientation(PageOrientation::PORTRAIT),
			m_pageMargin(),
			m_backgrounds(false),
			m_status(Job::Status::PENDING),
			m_callback()
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

		CefPdfPrintSettings Job::GetCefPdfPrintSettings() const
		{
			CefPdfPrintSettings pdfSettings;

			pdfSettings.backgrounds_enabled = m_backgrounds;
			pdfSettings.landscape = (m_pageOrientation == PageOrientation::LANDSCAPE);

			pdfSettings.page_width = m_pageSize.width * 1000;
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
