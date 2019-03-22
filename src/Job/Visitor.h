#ifndef JOB_VISITOR_H_
#define JOB_VISITOR_H_

#include "include/cef_base.h"

namespace cefpdf
{
	namespace job
	{

		class Local;
		class Remote;
		class StdInput;

		class Visitor : public CefBaseRefCounted
		{

		public:
			Visitor() {}

			virtual void visit(CefRefPtr<Local>) = 0;
			virtual void visit(CefRefPtr<Remote>) = 0;
			virtual void visit(CefRefPtr<StdInput>) = 0;

		private:
			// Include the default reference counting implementation.
			IMPLEMENT_REFCOUNTING(Visitor)
		};

	} // namespace job
} // namespace cefpdf

#endif // JOB_VISITOR_H_
