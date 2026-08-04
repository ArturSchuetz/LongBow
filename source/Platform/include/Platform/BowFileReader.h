#pragma once
#include "Platform/BowPlatformPredeclares.h"
#include "Platform/Platform_api.h"

namespace bow
{

class PLATFORM_API FileReader
{
  public:
    FileReader();
    ~FileReader();

    bool Open(const char *filePath);

    void Close();

    size_t GetSizeOfFile();

    unsigned long Tell();

    void Seek(size_t offset);

    void Skip(long count);

    size_t Read(char *memory_block, size_t sizeInBytes);

    bool EndOfFile() const;

  private:
    void *m_pFile;
};
/*----------------------------------------------------------------*/
} // namespace bow
