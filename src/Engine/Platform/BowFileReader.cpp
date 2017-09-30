#include "BowFileReader.h"

namespace Bow
{
	namespace Platform
	{
		bool FileReader::Open(const char* file)
		{
			return false;
		}

		unsigned int FileReader::Tell()
		{
			return 0;
		}

		void FileReader::Seek(size_t offset)
		{

		}

		void FileReader::Skip(long count)
		{

		}

		size_t FileReader::Read(char* memory_block, size_t sizeInBytes)
		{

			// return how much was read
			return 0;
		}

		void FileReader::Close()
		{

		}

		bool FileReader::EndOfFile() const
		{
			return true;

		}
	}
}