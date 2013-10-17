#pragma once
#include "LongBow.h"
#include "OGLBuffer.h"

#include <GL\glew.h>

namespace Bow {

	class OGLPixelBuffer
    {
	public:
		OGLPixelBuffer(GLenum target, BufferHint usageHint, int sizeInBytes);

        void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		//void CopyFromBitmap(Bitmap bitmap)

        VoidPtr CopyToSystemMemory(int offsetInBytes, int sizeInBytes);
		//Bitmap CopyToBitmap(int width, int height, ImagingPixelFormat pixelFormat);
		
		int GetSizeInBytes() const;
		BufferHint GetUsageHint() const;

		OGLBufferNamePtr GetHandle() const;
        void Bind() const;

	private:
        OGLBufferNamePtr	m_name;
		const int			m_sizeInBytes;
        const GLenum		m_target;
        const GLenum		m_usageHint;
    };

	typedef std::shared_ptr<OGLPixelBuffer> OGLPixelBufferPtr;
}