#include "BowFileWriter.h"

namespace Bow
{
	namespace Platform
	{
		bool FileWriter::Open(const char* file)
		{
			return false;
		}

		unsigned int FileWriter::Tell()
		{
			return 0;
		}

		void FileWriter::Seek(size_t offset)
		{

		}

		void FileWriter::Skip(long count)
		{

		}

		size_t FileWriter::Write(char* memory_block, size_t sizeInBytes)
		{
			return 0;
		}

		void FileWriter::Close()
		{

		}

		bool FileWriter::EndOfFile() const
		{
			return true;
		}
	}
}