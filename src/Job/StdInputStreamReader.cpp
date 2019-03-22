#include "StdInputStreamReader.h"

namespace cefpdf
{
	namespace job
	{

		StdInputStreamReader::StdInputStreamReader()
		{}

		int StdInputStreamReader::Eof()
		{
			return std::cin.eof() ? 1 : 0;
		}

		bool StdInputStreamReader::MayBlock()
		{
			return true;
		}

		std::size_t StdInputStreamReader::Read(void* ptr, std::size_t size, std::size_t n)
		{
			std::cin.read(static_cast<char*>(ptr), size);
			return std::cin.gcount();
		}

		int StdInputStreamReader::Seek(int64 offset, int whence)
		{
			std::ios_base::seekdir dir;

			switch (whence)
			{
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

		int64 StdInputStreamReader::Tell()
		{
			return std::cin.tellg();
		}

	} // namespace job

} // namespace cefpdf
