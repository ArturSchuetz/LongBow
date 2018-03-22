#include "BowDirectX12RenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "IBowTexture2D.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"

#include <string>
#include <wrl.h>
#include <shellapi.h>

namespace bow {
	
	DirectX12RenderDevice::DirectX12RenderDevice(void) 
	{
	}

	DirectX12RenderDevice::~DirectX12RenderDevice(void)
	{
		VRelease();
	}

	bool DirectX12RenderDevice::Initialize(void)
	{

		IDXGIFactory4* dxgiFactory;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
		if (FAILED(hr))
		{
			LOG_ERROR("Error while creating DXGI Factory");
			return false;
		}

        return true;
	}

	void DirectX12RenderDevice::VRelease(void)
	{
	}

	GraphicsWindowPtr DirectX12RenderDevice::VCreateWindow(int width, int height, const std::string& title, WindowType type)
	{
		return GraphicsWindowPtr(nullptr);
	}

	// =========================================================================
	// SHADER STUFF:
	// =========================================================================
	ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr DirectX12RenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	MeshBufferPtr DirectX12RenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		return MeshBufferPtr(nullptr);
	}

	VertexBufferPtr	DirectX12RenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
	{
		return VertexBufferPtr(nullptr);
	}

	IndexBufferPtr DirectX12RenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
	{
		return IndexBufferPtr(nullptr);
	}

	WritePixelBufferPtr DirectX12RenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
	{
		return WritePixelBufferPtr(nullptr);
	}

	Texture2DPtr DirectX12RenderDevice::VCreateTexture2D(Texture2DDescription description)
	{
		return Texture2DPtr(nullptr);
	}

	Texture2DPtr DirectX12RenderDevice::VCreateTexture2D(ImagePtr image)
	{
		return Texture2DPtr(nullptr);
	}

	TextureSamplerPtr DirectX12RenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
	{
		return TextureSamplerPtr(nullptr);
	}

}
