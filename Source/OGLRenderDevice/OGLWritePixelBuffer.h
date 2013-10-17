#pragma once
#include "IBowWritePixelBuffer.h"
#include "OGLPixelBuffer.h"

namespace Bow {

	class OGLWritePixelBuffer : public IWritePixelBuffer
    {
	public:
		OGLWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes);

        void Bind();
        static void UnBind();

        void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
        VoidPtr CopyToSystemMemory(int offsetInBytes, int sizeInBytes);

        int				GetSizeInBytes() const;
        PixelBufferHint	GetUsageHint() const;

	private:
		const PixelBufferHint m_usageHint;
		OGLPixelBuffer m_bufferObject;
    };

	typedef std::shared_ptr<OGLWritePixelBuffer> OGLWritePixelBufferPtr;

}