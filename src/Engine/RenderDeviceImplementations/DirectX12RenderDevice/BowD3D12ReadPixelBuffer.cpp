#include "BowD3D12ReadPixelBuffer.h"
#include "BowLogger.h"

#include "BowBufferHint.h"

namespace bow {

	BufferHint rbp_bufferHints[] = {
		BufferHint::StreamRead,
		BufferHint::StaticRead,
		BufferHint::DynamicRead
	};


	D3DReadPixelBuffer::D3DReadPixelBuffer(D3DRenderDevice* device, PixelBufferHint usageHint, int sizeInBytes) :
		m_device(device),
		m_UsageHint(usageHint), 
		m_BufferObject(device, rbp_bufferHints[(int)usageHint], sizeInBytes)
	{

	}

	void D3DReadPixelBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}

	std::shared_ptr<void> D3DReadPixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}

	int	D3DReadPixelBuffer::VGetSizeInBytes() const
	{
		return m_BufferObject.GetSizeInBytes();
	}

	PixelBufferHint	D3DReadPixelBuffer::VGetUsageHint() const
	{
		return m_UsageHint;
	}

}
