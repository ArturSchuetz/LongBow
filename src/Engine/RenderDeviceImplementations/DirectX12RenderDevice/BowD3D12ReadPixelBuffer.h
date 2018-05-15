#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowReadPixelBuffer.h"
#include "BowD3D12PixelBuffer.h"

namespace bow {

	class D3DReadPixelBuffer : public IReadPixelBuffer
	{
	public:
		D3DReadPixelBuffer(D3DRenderDevice* device, PixelBufferHint usageHint, int sizeInBytes);

		void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

		int	VGetSizeInBytes() const;
		PixelBufferHint	VGetUsageHint() const;

	private:
		D3DRenderDevice*		m_device;
		const PixelBufferHint m_UsageHint;
		D3DPixelBuffer m_BufferObject;
	};

}
