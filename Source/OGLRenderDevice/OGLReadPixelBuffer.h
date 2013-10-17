#pragma once
#include "IBowReadPixelBuffer.h"
#include "OGLPixelBuffer.h"

namespace Bow {

	class OGLReadPixelBuffer : public IReadPixelBuffer
    {
	public:
		OGLReadPixelBuffer(PixelBufferHint usageHint, int sizeInBytes);
        void Bind();

        void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
        VoidPtr CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

        int	GetSizeInBytes() const;
        PixelBufferHint	GetUsageHint() const;

	private:
		const PixelBufferHint m_usageHint;
		OGLPixelBuffer m_bufferObject;
    };

	typedef std::shared_ptr<OGLReadPixelBuffer> OGLReadPixelBufferPtr;

}