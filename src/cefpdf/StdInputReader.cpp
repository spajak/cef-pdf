// Copyright (c) 2021, Sebastian Pająk. MIT License.

#include "StdInputReader.h"

namespace cefpdf {

StdInputReader::StdInputReader()
{}

int StdInputReader::Eof()
{
    return std::cin.eof() ? 1 : 0;
}

bool StdInputReader::MayBlock()
{
    return true;
}

std::size_t StdInputReader::Read(void* ptr, std::size_t size, std::size_t n)
{
    std::cin.read(static_cast<char*>(ptr), size);
    return std::cin.gcount();
}

int StdInputReader::Seek(int64 offset, int whence)
{
    std::ios_base::seekdir dir;

    switch (whence) {
        case SEEK_CUR:
            dir = std::ios_base::cur;
        break;

        case SEEK_END:
            dir = std::ios_base::end;
        break;

        default:
            dir = std::ios_base::beg;
    }

    std::cin.seekg(offset, dir);
    return std::cin.good() ? 0 : 1;
}

int64 StdInputReader::Tell()
{
    return std::cin.tellg();
}

} // namespace cefpdf
