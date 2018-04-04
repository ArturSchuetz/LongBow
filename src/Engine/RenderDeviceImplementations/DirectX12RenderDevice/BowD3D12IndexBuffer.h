#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowIndexBuffer.h"
#include "BowIndexBufferDatatype.h"

// DirectX 12 specific headers.
#include <d3d12.h>

namespace bow {

	class D3DIndexBuffer : public IIndexBuffer
	{
	public:
		D3DIndexBuffer(D3DRenderDevice* device, BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes);
		~D3DIndexBuffer();

		int GetCount();

		void VCopyFromSystemMemory(void* bufferInSystemMemory, int destinationOffsetInBytes, int lengthInBytes);
		std::shared_ptr<void> VCopyToSystemMemory(int offsetInBytes, int sizeInBytes);

		int			VGetSizeInBytes();
		BufferHint	VGetUsageHint();
		IndexBufferDatatype GetDatatype();
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

	private:
		D3DRenderDevice*		m_device;
		ComPtr<ID3D12Resource>	m_buffer;

		const IndexBufferDatatype m_datatype;
		const BufferHint m_usageHint;
		const int m_sizeInBytes;
	};

}
