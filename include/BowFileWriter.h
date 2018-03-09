#pragma once
#include "BowPrerequisites.h"
#include "BowPlatformPredeclares.h"

namespace bow {

	class FileWriter
	{
	public:
		FileWriter();
		~FileWriter();

		bool Open(const char* filePath);

		void Close();

		size_t GetSizeOfFile();

		unsigned long Tell();

		void Seek(size_t offset);

		void Skip(long count);

		size_t Write(char* memory_block, size_t sizeInBytes);

		bool EndOfFile() const;

	private:
		void* m_pFile;
	};
	/*----------------------------------------------------------------*/
}
