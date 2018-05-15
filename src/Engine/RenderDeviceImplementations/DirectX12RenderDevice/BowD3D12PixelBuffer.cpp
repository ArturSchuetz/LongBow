#include "BowD3D12PixelBuffer.h"
#include "BowLogger.h"

#include "BowD3D12TypeConverter.h"

namespace bow {

	D3DPixelBuffer::D3DPixelBuffer(D3DRenderDevice* device, BufferHint usageHint, int sizeInBytes) :
		m_device(device),
		m_sizeInBytes(sizeInBytes),
		m_usageHint(usageHint)
	{

	}


	void D3DPixelBuffer::CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{

	}


	std::shared_ptr<void> D3DPixelBuffer::CopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return std::shared_ptr<void>(nullptr);
	}


	int D3DPixelBuffer::GetSizeInBytes() const
	{
		return m_sizeInBytes;
	}


	BufferHint D3DPixelBuffer::GetUsageHint() const
	{
		return m_usageHint;
	}

}
