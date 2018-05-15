#include "BowD3D12Texture2D.h"
#include "BowLogger.h"

#include "BowD3D12RenderDevice.h"

#include "BowD3D12TextureSampler.h"
#include "BowD3D12TypeConverter.h"

namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);
	extern std::string toErrorString(HRESULT hresult);

	D3DTexture2D::D3DTexture2D(D3DRenderDevice* device, Texture2DDescription description) : 
		m_device(device), 
		m_Description(description)
	{
		// Describe and create a Texture2D.
		m_textureDesc = {};
		m_textureDesc.MipLevels = 1;
		m_textureDesc.Format = D3DTypeConverter::To(description.GetTextureFormat());
		m_textureDesc.Width = description.GetWidth();
		m_textureDesc.Height = description.GetHeight();
		m_textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		m_textureDesc.DepthOrArraySize = 1;
		m_textureDesc.SampleDesc.Count = 1;
		m_textureDesc.SampleDesc.Quality = 0;
		m_textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	}

	D3DTexture2D::~D3DTexture2D()
	{

	}

	void D3DTexture2D::VCopyFromBuffer(WritePixelBufferPtr pixelBuffer, int xOffset, int yOffset, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{

	}

	void D3DTexture2D::VCopyFromSystemMemory(void* bitmapInSystemMemory, int width, int height, ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		HRESULT hresult;

		LOG_ASSERT(!((width) > m_Description.GetWidth()), "xOffset + width must be less than or equal to Description.Width");
		LOG_ASSERT(!((height) > m_Description.GetHeight()), "yOffset + height must be less than or equal to Description.Height");

		VerifyRowAlignment(rowAlignment);

		// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
		// the command list that references it has finished executing on the GPU.
		// We will flush the GPU at the end of this method to ensure the resource is not
		// prematurely destroyed.
		ComPtr<ID3D12Resource> textureUploadHeap;

		// Create an committed resource for the upload.
		if (bitmapInSystemMemory)
		{
			m_currentState = D3D12_RESOURCE_STATE_COPY_DEST;
			hresult = m_device->m_d3d12device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&m_textureDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_texture));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateCommittedResource: ") + errorMsg).c_str());
			}

			const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);

			// Create the GPU upload buffer.
			hresult = m_device->m_d3d12device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&textureUploadHeap));

			ComPtr<ID3D12GraphicsCommandList2> commandList = m_device->GetCopyCommandList();

			unsigned int sizePerPixel = SizeInBytes(dataType) * NumberOfChannels(format);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = bitmapInSystemMemory;
			subresourceData.RowPitch = width * sizePerPixel;
			subresourceData.SlicePitch = subresourceData.RowPitch * height;

			UpdateSubresources(commandList.Get(), m_texture.Get(), textureUploadHeap.Get(), 0, 0, 1, &subresourceData);

			// This can be implemented deferred (collect all commands and execute them all together instead of executing them one by one)
			auto fenceValue = m_device->ExecuteCopyCommandList(commandList);
			m_device->WaitForCopyFenceValue(fenceValue);

			ComPtr<ID3D12GraphicsCommandList2> directCommandList = m_device->GetDirectCommandList();
			directCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), m_currentState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
			m_currentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;			
			
			fenceValue = m_device->ExecuteDirectCommandList(directCommandList);
			m_device->WaitForDirectFenceValue(fenceValue);
		}
	}

	std::shared_ptr<void> D3DTexture2D::VCopyToSystemMemory(ImageFormat format, ImageDatatype dataType, int rowAlignment)
	{
		return std::shared_ptr<unsigned char>(nullptr);
	}

	Texture2DDescription D3DTexture2D::VGetDescription()
	{
		return m_Description;
	}
}
