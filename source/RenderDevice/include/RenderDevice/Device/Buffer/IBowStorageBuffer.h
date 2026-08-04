#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IStorageBuffer
{
  public:
    virtual void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t lengthInBytes) { VCopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

    virtual void VCopyFromSystemMemory(void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) = 0;

    virtual std::shared_ptr<void> VCopyToSystemMemory(int64_t offsetInBytes, int64_t sizeInBytes) = 0;

    virtual int64_t VGetSizeInBytes() = 0;
    virtual BufferHint VGetUsageHint() = 0;
};

} // namespace bow
