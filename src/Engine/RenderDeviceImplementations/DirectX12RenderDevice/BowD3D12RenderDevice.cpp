#include "BowD3D12RenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"

#include "IBowTexture2D.h"

#include "BowD3D12GraphicsWindow.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);
	extern std::string toErrorString(HRESULT hresult);

	D3DRenderDevice::D3DRenderDevice(void) : 
		m_D3D12device(nullptr),
		m_useWarpDevice(true),
		m_initialized(false)
	{
		m_hInstance = GetModuleHandle(NULL);
	}

	D3DRenderDevice::~D3DRenderDevice(void)
	{
		VRelease();
	}

	bool D3DRenderDevice::Initialize(void)
	{
		HRESULT hresult;

		UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			ComPtr<ID3D12Debug> debugController = nullptr;
			hresult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			if (SUCCEEDED(hresult))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		ComPtr<IDXGIAdapter4> dxgiAdapter4;
		hresult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("EnumWarpAdapter: ") + errorMsg).c_str());
			return false;
		}

		if (m_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter = nullptr;
			hresult = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("EnumWarpAdapter: ") + errorMsg).c_str());
				return false;
			}
			warpAdapter.As(&dxgiAdapter4);
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter = nullptr;
			{
				IDXGIAdapter1** ppAdapter = &hardwareAdapter;
				*ppAdapter = nullptr;

				IDXGIAdapter1* adapter;
				for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
				{
					DXGI_ADAPTER_DESC1 desc;
					adapter->GetDesc1(&desc);

					if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					{
						// Don't select the Basic Render Driver adapter.
						// If you want a software adapter, pass in "/warp" on the command line.
						continue;
					}

					// Check to see if the adapter supports Direct3D 12, but don't create the
					// actual device yet.
					hresult = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
					if (SUCCEEDED(hresult))
					{
						break;
					}
				}
				*ppAdapter = adapter;
			}

			hardwareAdapter.As(&dxgiAdapter4);
		}

		hresult = D3D12CreateDevice(dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_D3D12device));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("D3D12CreateDevice: ") + errorMsg).c_str());
			return false;
		}

		// Enable debug messages in debug mode.
#if defined(_DEBUG)
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(m_D3D12device.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			hresult = pInfoQueue->PushStorageFilter(&NewFilter);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("PushStorageFilter: ") + errorMsg).c_str());
				return false;
			}
		}
#endif

        return true;
	}

	void D3DRenderDevice::VRelease(void)
	{    

	}

	GraphicsWindowPtr D3DRenderDevice::VCreateWindow(int width, int height, const std::string& title, WindowType type)
	{
		D3DGraphicsWindowPtr pGraphicsWindow = D3DGraphicsWindowPtr(new D3DGraphicsWindow());
		if (pGraphicsWindow->Initialize(width, height, title, type, this))
		{
			// Test for properties here

			return pGraphicsWindow;
		}
		else
		{
			LOG_ERROR("Error while creating DirectX-Window!");
			return GraphicsWindowPtr(nullptr);
		}
	}

	// =========================================================================
	// SHADER STUFF:
	// =========================================================================

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource)
	{
		return ShaderProgramPtr(nullptr);
	}

	MeshBufferPtr D3DRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		return MeshBufferPtr(nullptr);
	}

	VertexBufferPtr	D3DRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
	{
		return VertexBufferPtr(nullptr);
	}

	IndexBufferPtr D3DRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
	{
		return IndexBufferPtr(nullptr);
	}

	WritePixelBufferPtr D3DRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
	{
		return WritePixelBufferPtr(nullptr);
	}

	Texture2DPtr D3DRenderDevice::VCreateTexture2D(Texture2DDescription description)
	{
		return Texture2DPtr(nullptr);
	}

	Texture2DPtr D3DRenderDevice::VCreateTexture2D(ImagePtr image)
	{
		return Texture2DPtr(nullptr);
	}

	TextureSamplerPtr D3DRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
	{
		return TextureSamplerPtr(nullptr);
	}

}
