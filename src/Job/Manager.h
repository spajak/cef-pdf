#ifndef JOB_MANAGER_H_
#define JOB_MANAGER_H_

#include "Job.h"

#include "include/cef_browser.h"
#include "include/cef_load_handler.h"

#include <vector>
#include <queue>

namespace cefpdf
{
	namespace job
	{

		class Manager : public CefBaseRefCounted
		{

		public:
			Manager() {}

			std::size_t Queue(CefRefPtr<Job> job);

			void Assign(CefRefPtr<CefBrowser> browser);

			CefRefPtr<CefStreamReader> GetStreamReader(CefRefPtr<CefBrowser> browser);

			void Process(CefRefPtr<CefBrowser> browser, int httpStatusCode);

			void Finish(CefRefPtr<CefBrowser> browser, const CefString& path, bool ok);

			void Abort(CefRefPtr<CefBrowser> browser, CefLoadHandler::ErrorCode errorCode);

			void StopAll();

		private:
			std::queue<CefRefPtr<Job>> m_jobsQueue;

			struct BrowserJob
			{
				CefRefPtr<CefBrowser> browser;
				CefRefPtr<Job> job;
				CefRefPtr<CefStreamReader> streamReader;
			};

			std::vector<BrowserJob> m_jobs;

			typedef std::vector<BrowserJob>::iterator Iterator;

			Iterator Find(CefRefPtr<CefBrowser> browser);

			void Resolve(Manager::Iterator it, const Job::Status&);

			// Include the default reference counting implementation.
			IMPLEMENT_REFCOUNTING(Manager)
		};

	} // namespace job
} // namespace cefpdf

#endif // JOB_MANAGER_H_
