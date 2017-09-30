#pragma once
#include "BowPrerequisites.h"
#include "BowPlatformPredeclares.h"

namespace Bow {
	namespace Platform {

		class FileWriter
		{
		public:
			FileWriter() {}
			~FileWriter() { Close(); }

			bool Open(const char* file);

			void Close();

			unsigned int Tell();

			void Seek(size_t offset);

			void Skip(long count);

			size_t Write(char* memory_block, size_t sizeInBytes);

			bool EndOfFile() const;
		};
		/*----------------------------------------------------------------*/
	}
}
