#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

namespace bow {

	class D3DPixelBuffer
	{
	public:
		D3DPixelBuffer(D3DRenderDevice* device, BufferHint usageHint, int sizeInBytes);

		void CopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		//void CopyFromBitmap(Bitmap bitmap)

		std::shared_ptr<void> CopyToSystemMemory(int offsetInBytes, int sizeInBytes);
		//Bitmap CopyToBitmap(int width, int height, ImagingPixelFormat pixelFormat);

		int GetSizeInBytes() const;
		BufferHint GetUsageHint() const;

	private:
		D3DRenderDevice*		m_device;
		const int			m_sizeInBytes;
		const BufferHint	m_usageHint;
	};
}
