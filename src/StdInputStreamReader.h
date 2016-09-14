#ifndef STDINPUT_STREAM_READER_H_
#define STDINPUT_STREAM_READER_H_

#include "include/cef_stream.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <algorithm>

namespace cefpdf {

class StdInputStreamReader : public CefReadHandler
{
    public:

    StdInputStreamReader() {
        std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;
    };

    virtual int Eof() {
        return std::cin.eof() ? 1 : 0;
    };

    virtual bool MayBlock() {
        return true;
    };

    virtual std::size_t Read(void* ptr, std::size_t size, std::size_t n) {
        std::cin.read(static_cast<char*>(ptr), size);
        return std::cin.gcount();
    };

    virtual int Seek(int64 offset, int whence) {
        std::ios_base::seekdir dir =
            (whence == SEEK_CUR ? std::ios_base::cur :
                (whence == SEEK_END ? std::ios_base::end : std::ios_base::beg)
            );

        std::cin.seekg(offset, dir);
        return std::cin.good() ? 0 : 1;
    };

    virtual int64 Tell() {
        return std::cin.tellg();
    };

    private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(StdInputStreamReader);
};

} // namespace cefpdf

#endif // STDINPUT_STREAM_READER_H_