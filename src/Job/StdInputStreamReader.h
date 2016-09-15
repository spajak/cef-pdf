#ifndef JOB_STDINPUT_STREAM_READER_H_
#define JOB_STDINPUT_STREAM_READER_H_

#include "include/cef_stream.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <algorithm>

namespace cefpdf {
namespace job {

class StdInputStreamReader : public CefReadHandler
{
    public:

    StdInputStreamReader() : m_initialized(false) {};

    void Initialize();

    virtual int Eof() {
        return std::cin.eof() ? 1 : 0;
    };

    virtual bool MayBlock() {
        return true;
    };

    virtual std::size_t Read(void* ptr, std::size_t size, std::size_t n);

    virtual int Seek(int64 offset, int whence);

    virtual int64 Tell() {
        return std::cin.tellg();
    };

    private:

    bool m_initialized;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInputStreamReader);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_STDINPUT_STREAM_READER_H_