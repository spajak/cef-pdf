#ifndef JOB_H_
#define JOB_H_

#include "Common.h"

#include "include/cef_load_handler.h"

#include <functional>

namespace cefpdf {

class Job : public CefBase
{
    public:

    friend class JobsManager;

    enum struct Status { Pending = 0, Loading, LoadError, Printing, Failed, Success };

    typedef CefLoadHandler::ErrorCode ErrorCode;

    typedef std::function<std::string()> ContentProvider;

    typedef std::function<void(Status, ErrorCode)> StatusCallback;

    Job();
    Job(const CefString& url);

    int GetID() const;

    const CefString& GetUrl() const;

    void SetUrl(const CefString& url);

    const CefString& GetContent();

    void SetContent(const CefString& content);

    void SetContentProvider(ContentProvider const& provider);

    const CefString& GetOutputPath() const;

    void SetOutputPath(const CefString& outputPath);


    void SetPageSize(const CefString& pageSize);

    void SetLandscape(bool flag = true);

    void SetPageMargin(const CefString& margin);

    // Get prepared PDF setting for CEF
    CefPdfPrintSettings GetCefPdfPrintSettings() const;

    Status GetStatus() const;
    ErrorCode GetStatusError() const;
    void OnStatus(StatusCallback const& callback);

    private:

    void Init();

    void SetStatus(Status status);

    void SetStatus(Status status, ErrorCode errorCode);

    void ParseCustomPageSize(const CefString& pageSize);

    void ParseCustomPageMargin(const CefString& pageMargin);

    static int counter;
    int m_id;

    Status m_status;
    ErrorCode m_errorCode;
    StatusCallback m_statusCallback;

    CefString m_url;
    CefString m_content;
    CefString m_outputPath;

    ContentProvider m_contentProvider;

    PageSize m_pageSize;
    PageOrientation m_pageOrientation = PageOrientation::PORTRAIT;
    PageMargin m_pageMargin;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Job);
};

} // namespace cefpdf

#endif // JOB_H_
