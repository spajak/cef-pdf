// Copyright (c) 2021, Sebastian Pająk. MIT License.

#ifndef CEFPDF_STDINPUT_STREAM_READER_H_
#define CEFPDF_STDINPUT_STREAM_READER_H_

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#include "include/cef_stream.h"

namespace cefpdf {

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

} // namespace cefpdf

#endif // CEFPDF_STDINPUT_STREAM_READER_H_
