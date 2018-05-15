#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowTexture2D.h"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace bow {

	class D3DTexture2D : public ITexture2D
	{
	public:
		D3DTexture2D(D3DRenderDevice* device, Texture2DDescription description);
		~D3DTexture2D();

		void VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
		void VCopyFromSystemMemory(void* bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment);
		std::shared_ptr<void> VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment);

		Texture2DDescription VGetDescription();

	private:
		D3DRenderDevice* m_device;

		const Texture2DDescription		m_Description;
		ComPtr<ID3D12Resource>			m_texture;
		ComPtr<ID3D12DescriptorHeap>	m_shaderResourceViewHeap;
		D3D12_RESOURCE_DESC				m_textureDesc;
		D3D12_RESOURCE_STATES			m_currentState;
	};

	typedef std::shared_ptr<D3DTexture2D> D3DTexture2DPtr;
}
