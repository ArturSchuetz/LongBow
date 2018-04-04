#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowVertexBuffer.h"

// DirectX 12 specific headers.
#include <d3d12.h>

namespace bow {

	class D3DVertexBuffer : public IVertexBuffer
	{
	public:
		D3DVertexBuffer(D3DRenderDevice* device, BufferHint usageHint, int sizeInBytes);
		~D3DVertexBuffer();

		void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

		int			VGetSizeInBytes();
		BufferHint	VGetUsageHint();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

	private:
		D3DRenderDevice*		m_device;
		ComPtr<ID3D12Resource>	m_buffer;

		const BufferHint m_usageHint;
		const int m_sizeInBytes;
	};

}
