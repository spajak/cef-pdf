#ifndef JOB_STDINPUT_STREAM_READER_H_
#define JOB_STDINPUT_STREAM_READER_H_

#include "include/cef_stream.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

namespace cefpdf {
namespace job {

class StdInputStreamReader : public CefReadHandler
{

public:
    StdInputStreamReader();

    virtual int Eof() override;

    virtual bool MayBlock() override;

    virtual std::size_t Read(void* ptr, std::size_t size, std::size_t n) override;

    virtual int Seek(int64 offset, int whence) override;

    virtual int64 Tell() override;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInputStreamReader);
};

} // namespace job
} // namespace cefpdf

#endif // JOB_STDINPUT_STREAM_READER_H_
