#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

class IIndexBuffer
{
  public:
    virtual void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t lengthInBytes) { VCopyFromSystemMemory(bufferInSystemMemory, 0, lengthInBytes); }

    virtual void VCopyFromSystemMemory(const void *bufferInSystemMemory, int64_t destinationOffsetInBytes, int64_t lengthInBytes) = 0;

    virtual int64_t VGetSizeInBytes() = 0;
    virtual BufferHint VGetUsageHint() = 0;
};

} // namespace bow
