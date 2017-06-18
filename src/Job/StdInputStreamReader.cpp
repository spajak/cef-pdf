#include "StdInputStreamReader.h"

namespace cefpdf {
namespace job {

void StdInputStreamReader::Initialize() {
    if (!m_initialized) {
        std::cout << "Waiting for input until EOF (Unix: Ctrl+D, Windows: Ctrl+Z)" << std::endl;
        m_initialized = true;
    }
}

std::size_t StdInputStreamReader::Read(void* ptr, std::size_t size, std::size_t n)
{
    Initialize();
    std::cin.read(static_cast<char*>(ptr), size);
    return std::cin.gcount();
}

int StdInputStreamReader::Seek(int64 offset, int whence)
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

} // namespace job
} // namespace cefpdf
