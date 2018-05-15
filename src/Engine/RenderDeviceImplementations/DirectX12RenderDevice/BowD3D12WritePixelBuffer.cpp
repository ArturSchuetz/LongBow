#include "BowD3D12WritePixelBuffer.h"
#include "BowLogger.h"

#include "BowBufferHint.h"

namespace bow {

	BufferHint wpb_bufferHints[] = {
		BufferHint::StreamRead,
		BufferHint::StaticRead,
		BufferHint::DynamicRead
	};


	D3DWritePixelBuffer::D3DWritePixelBuffer(D3DRenderDevice* device, PixelBufferHint usageHint, int sizeInBytes) :
		m_device(device),
		m_UsageHint(usageHint), 
		m_BufferObject(device, wpb_bufferHints[(int)usageHint], sizeInBytes)
	{

	}

	void D3DWritePixelBuffer::VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes)
	{
		m_BufferObject.CopyFromSystemMemory(bufferInSystemMemory, destinationOffsetInBytes, lengthInBytes);
	}


	std::shared_ptr<void> D3DWritePixelBuffer::VCopyToSystemMemory(int offsetInBytes, int sizeInBytes)
	{
		return m_BufferObject.CopyToSystemMemory(offsetInBytes, sizeInBytes);
	}


	int	D3DWritePixelBuffer::VGetSizeInBytes() const
	{
		return m_BufferObject.GetSizeInBytes();
	}


	PixelBufferHint	D3DWritePixelBuffer::VGetUsageHint() const
	{
		return m_UsageHint;
	}

}
