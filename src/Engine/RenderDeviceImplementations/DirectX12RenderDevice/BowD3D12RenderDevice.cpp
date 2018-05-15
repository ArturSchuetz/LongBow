#include "BowD3D12RenderDevice.h"
#include "BowCore.h"
#include "BowResources.h"
#include "BowLogger.h"
#include "BowD3D12TypeConverter.h"

#include "BowD3D12GraphicsWindow.h"
#include "BowD3D12ShaderProgram.h"
#include "BowShaderVertexAttribute.h"
#include "BowVertexBufferAttribute.h"
#include "IBowVertexAttribute.h"

#include "IBowIndicesBase.h"
#include "BowD3D12IndexBuffer.h"
#include "BowD3D12VertexBuffer.h"
#include "BowD3D12PixelBuffer.h"
#include "BowD3D12WritePixelBuffer.h"
#include "BowD3D12ReadPixelBuffer.h"

#include "BowD3D12Texture2D.h"
#include "BowD3D12TextureSampler.h"

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
		m_d3d12device(nullptr),
		m_useWarpDevice(false),
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
		ComPtr<IDXGIFactory4> factory;
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debugController = nullptr;

		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		{
			hresult = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			if (SUCCEEDED(hresult))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
			else
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("D3D12GetDebugInterface: ") + errorMsg).c_str());
			}
		}
#endif

		ComPtr<IDXGIAdapter1> dxgiAdapter1 = nullptr;
		ComPtr<IDXGIAdapter4> dxgiAdapter4 = nullptr;

		hresult = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateDXGIFactory2: ") + errorMsg).c_str());
			return false;
		}

		if (m_useWarpDevice)
		{
			hresult = factory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("factory->EnumWarpAdapter: ") + errorMsg).c_str());
				return false;
			}

			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			hresult = dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("dxgiAdapter1->GetDesc1: ") + errorMsg).c_str());
			}

			LOG_INFO(std::string("Using WARP Device: " + widestring2string(dxgiAdapterDesc1.Description)).c_str());
			LOG_INFO(std::string("\t\tDedicated Video Memory:  " + std::to_string(dxgiAdapterDesc1.DedicatedVideoMemory / 1024 / 1024) + " MB").c_str());
			LOG_INFO(std::string("\t\tDedicated System Memory: " + std::to_string(dxgiAdapterDesc1.DedicatedSystemMemory / 1024 / 1024) + " MB").c_str());
			LOG_INFO(std::string("\t\tShared System Memory:    " + std::to_string(dxgiAdapterDesc1.SharedSystemMemory / 1024 / 1024) + " MB").c_str());

			hresult = dxgiAdapter1.As(&dxgiAdapter4);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("dxgiAdapter1.As: ") + errorMsg).c_str());
				return false;
			}
		}
		else
		{
			SIZE_T maxDedicatedVideoMemory = 0;
			ComPtr<IDXGIAdapter1> choosedDevice = nullptr;
			LOG_INFO("Graphics Devices found:");
			for (UINT i = 0; factory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
				dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);
				if (FAILED(hresult))
				{
					std::string errorMsg = toErrorString(hresult);
					LOG_ERROR(std::string(std::string("dxgiAdapter1->GetDesc1: ") + errorMsg).c_str());
				}

				LOG_INFO(std::string("\t" + widestring2string(dxgiAdapterDesc1.Description) + ":").c_str());
				LOG_INFO(std::string("\t\tDedicated Video Memory:  " + std::to_string(dxgiAdapterDesc1.DedicatedVideoMemory / 1024 / 1024) + " MB").c_str());
				LOG_INFO(std::string("\t\tDedicated System Memory: " + std::to_string(dxgiAdapterDesc1.DedicatedSystemMemory / 1024 / 1024) + " MB").c_str());
				LOG_INFO(std::string("\t\tShared System Memory:    " + std::to_string(dxgiAdapterDesc1.SharedSystemMemory / 1024 / 1024) + " MB").c_str());

				// Check to see if the adapter can create a D3D12 device without actually 
				// creating it. The adapter with the largest dedicated video memory
				// is favored.

				hresult = D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
				if (SUCCEEDED(hresult))
				{
					if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 && dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
					{
						maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
						choosedDevice = dxgiAdapter1;

						hresult = dxgiAdapter1.As(&dxgiAdapter4);
						if (FAILED(hresult))
						{
							std::string errorMsg = toErrorString(hresult);
							LOG_ERROR(std::string(std::string("dxgiAdapter1.As: ") + errorMsg).c_str());
							return false;
						}
					}
				}
			}

			if (dxgiAdapter4 == nullptr)
			{
				// If no hardware device was found, warp should be used
				m_useWarpDevice = true;
				// retry with warp devices
				return Initialize();
			}
			else
			{
				DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
				choosedDevice->GetDesc1(&dxgiAdapterDesc1);
				LOG_INFO(std::string("Using HARDWARE Device: " + widestring2string(dxgiAdapterDesc1.Description)).c_str());
			}
		}

		hresult = D3D12CreateDevice(dxgiAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12device));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("D3D12CreateDevice: ") + errorMsg).c_str());
			return false;
		}

		// Enable debug messages in debug mode.
#if defined(_DEBUG)
		ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
		if (SUCCEEDED(m_d3d12device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

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

			hresult = infoQueue->PushStorageFilter(&NewFilter);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("infoQueue->PushStorageFilter: ") + errorMsg).c_str());
				return false;
			}
		}
#endif
		m_copyCommandLists = new CommandQueueCollection(m_d3d12device, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY);
		m_directCommandLists = new CommandQueueCollection(m_d3d12device, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_computeCommandLists = new CommandQueueCollection(m_d3d12device, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
        return m_copyCommandLists->Initialize() && m_directCommandLists->Initialize() && m_computeCommandLists->Initialize();
	}

	void D3DRenderDevice::VRelease(void)
	{    
		if (m_copyCommandLists != nullptr)
		{
			delete m_copyCommandLists; 
			m_copyCommandLists = nullptr;
		}

		if (m_directCommandLists != nullptr)
		{
			delete m_directCommandLists;
			m_directCommandLists = nullptr;
		}

		if (m_computeCommandLists != nullptr)
		{
			delete m_computeCommandLists;
			m_computeCommandLists = nullptr;
		}
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
		//Open file
		std::string vshaderString;
		std::ifstream vsourceFile(VertexShaderFilename.c_str());

		//Source file loaded
		if (vsourceFile)
		{
			//Get shader source
			vshaderString.assign((std::istreambuf_iterator< char >(vsourceFile)), std::istreambuf_iterator<char>());

			//Open file
			std::string fshaderString;
			std::ifstream fsourceFile(FragementShaderFilename.c_str());

			if (fsourceFile)
			{
				//Get shader source
				fshaderString.assign((std::istreambuf_iterator< char >(fsourceFile)), std::istreambuf_iterator<char>());
				return VCreateShaderProgram(vshaderString, fshaderString);
			}
			else
			{
				LOG_ERROR("Could not open Shader from File");
				return D3DShaderProgramPtr();
			}
		}
		else
		{
			LOG_ERROR("Could not open Shader from File");
			return D3DShaderProgramPtr();
		}
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgramFromFile(const std::string& VertexShaderFilename, const std::string& GeometryShaderFilename, const std::string& FragementShaderFilename)
	{
		//Open file
		std::string vshaderString;
		std::ifstream vsourceFile(VertexShaderFilename.c_str());

		//Source file loaded
		if (vsourceFile)
		{
			//Get shader source
			vshaderString.assign((std::istreambuf_iterator<char>(vsourceFile)), std::istreambuf_iterator<char>());

			//Open file
			std::string gshaderString;
			std::ifstream gsourceFile(GeometryShaderFilename.c_str());

			if (gsourceFile)
			{
				//Get shader source
				gshaderString.assign((std::istreambuf_iterator<char>(gsourceFile)), std::istreambuf_iterator<char>());

				//Open file
				std::string fshaderString;
				std::ifstream fsourceFile(FragementShaderFilename.c_str());

				if (fsourceFile)
				{
					//Get shader source
					fshaderString.assign((std::istreambuf_iterator<char>(fsourceFile)), std::istreambuf_iterator<char>());
					return VCreateShaderProgram(vshaderString, gshaderString, fshaderString);
				}
				else
				{
					LOG_ERROR("Could not open Shader from File");
					return D3DShaderProgramPtr(nullptr);
				}
			}
			else
			{
				LOG_ERROR("Could not open Shader from File");
				return D3DShaderProgramPtr(nullptr);
			}
		}
		else
		{
			LOG_ERROR("Could not open Shader from File");
			return D3DShaderProgramPtr(nullptr);
		}
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& FragementShaderSource)
	{
		D3DShaderProgram *shaderProgram = new D3DShaderProgram();
		shaderProgram->Initialize(m_d3d12device, VertexShaderSource, FragementShaderSource);
		return ShaderProgramPtr(shaderProgram);
	}

	ShaderProgramPtr D3DRenderDevice::VCreateShaderProgram(const std::string& VertexShaderSource, const std::string& GeometryShaderSource, const std::string& FragementShaderSource)
	{
		D3DShaderProgram *shaderProgram = new D3DShaderProgram();
		shaderProgram->Initialize(m_d3d12device, VertexShaderSource, FragementShaderSource);
		return ShaderProgramPtr(shaderProgram);
	}

	MeshBufferPtr D3DRenderDevice::VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint)
	{
		return MeshBufferPtr(nullptr);
	}

	VertexBufferPtr	D3DRenderDevice::VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes)
	{
		return D3DVertexBufferPtr(new D3DVertexBuffer(this, usageHint, sizeInBytes));
	}

	IndexBufferPtr D3DRenderDevice::VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes)
	{
		return D3DIndexBufferPtr(new D3DIndexBuffer(this, usageHint, dataType, sizeInBytes));
	}

	WritePixelBufferPtr D3DRenderDevice::VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes)
	{
		return D3DWritePixelBufferPtr(new D3DWritePixelBuffer(this, usageHint, sizeInBytes));
	}

	Texture2DPtr D3DRenderDevice::VCreateTexture2D(Texture2DDescription description)
	{
		return D3DTexture2DPtr(new D3DTexture2D(this, description));
	}

	Texture2DPtr D3DRenderDevice::VCreateTexture2D(ImagePtr image)
	{
		if (image->GetSizeInBytes() == 0)
			return Texture2DPtr(nullptr);

		TextureFormat format;
		if (image->GetSizeInBytes() / (image->GetHeight() * image->GetWidth()) == 3)
			format = TextureFormat::RedGreenBlue8;
		else
			format = TextureFormat::RedGreenBlueAlpha8;

		D3DTexture2DPtr textureD3D = D3DTexture2DPtr(new D3DTexture2D(this, Texture2DDescription(image->GetWidth(), image->GetHeight(), format, true)));

		Texture2DPtr texture = std::dynamic_pointer_cast<ITexture2D>(textureD3D);
		texture->VCopyFromSystemMemory(image->GetData(), D3DTypeConverter::TextureToImageFormat(format), ImageDatatype::UnsignedByte);

		return texture;
	}

	TextureSamplerPtr D3DRenderDevice::VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
	{
		return D3DTextureSamplerPtr(new D3DTextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy));
	}

	ComPtr<ID3D12GraphicsCommandList2> D3DRenderDevice::GetCopyCommandList()
	{
		return m_copyCommandLists->GetCommandList();
	}

	ComPtr<ID3D12GraphicsCommandList2> D3DRenderDevice::GetDirectCommandList()
	{
		return m_directCommandLists->GetCommandList();
	}

	ComPtr<ID3D12GraphicsCommandList2> D3DRenderDevice::GetComputeCommandList()
	{
		return m_computeCommandLists->GetCommandList();
	}

	uint64_t D3DRenderDevice::ExecuteCopyCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		return m_copyCommandLists->ExecuteCommandList(commandList);
	}

	uint64_t D3DRenderDevice::ExecuteDirectCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		return m_directCommandLists->ExecuteCommandList(commandList);
	}

	uint64_t D3DRenderDevice::ExecuteComputeCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		return m_computeCommandLists->ExecuteCommandList(commandList);
	}

	void D3DRenderDevice::WaitForCopyFenceValue(uint64_t fanceValue)
	{
		return m_copyCommandLists->WaitForFenceValue(fanceValue);
	}

	void D3DRenderDevice::WaitForDirectFenceValue(uint64_t fanceValue)
	{
		return m_directCommandLists->WaitForFenceValue(fanceValue);
	}

	void D3DRenderDevice::WaitForComputeFenceValue(uint64_t fanceValue)
	{
		return m_computeCommandLists->WaitForFenceValue(fanceValue);
	}

	// ====================================================================
	// CommandQueueCollection
	// ====================================================================

	bool CommandQueueCollection::Initialize()
	{
		m_commandQueue = D3DRenderDevice::CreateCommandQueue(m_d3d12device, m_commandListType);
		if (m_commandQueue == nullptr)
			return false;

		m_fence = D3DRenderDevice::CreateFence(m_d3d12device);
		if (m_fence == nullptr)
			return false;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = D3DRenderDevice::CreateEventHandle();
		if (m_fenceEvent == nullptr)
			return false;
	}

	ComPtr<ID3D12GraphicsCommandList2> CommandQueueCollection::GetCommandList()
	{
		HRESULT hresult;
		ID3D12CommandAllocator* allocator;
		ComPtr<ID3D12GraphicsCommandList2> commandList;

		if (!m_availableCommandAllocatorsQueue.empty() && D3DRenderDevice::IsFenceComplete(m_fence, m_availableCommandAllocatorsQueue.front().first))
		{
			allocator = m_availableCommandAllocatorsQueue.front().second;
			m_availableCommandAllocatorsQueue.pop();

			hresult = allocator->Reset();
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("commandAllocator->Reset: ") + errorMsg).c_str());
			}
		}
		else
		{
			ComPtr<ID3D12CommandAllocator> newAllocator = D3DRenderDevice::CreateCommandAllocator(m_d3d12device, m_commandListType);
			m_allAllocators.push_back(newAllocator);
			allocator = newAllocator.Get();
		}

		if (!m_availableCommandListsQueue.empty())
		{
			commandList = m_availableCommandListsQueue.front();
			m_availableCommandListsQueue.pop();

			hresult = commandList->Reset(allocator, nullptr);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("commandList->Reset: ") + errorMsg).c_str());
			}
		}
		else
		{
			commandList = D3DRenderDevice::CreateCommandList(m_d3d12device, allocator, m_commandListType);
		}

		// Associate the command allocator with the command list so that it can be
		// retrieved when the command list is executed.
		hresult = commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), allocator);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandList->SetPrivateDataInterface: ") + errorMsg).c_str());
		}

		return commandList;
	}

	uint64_t CommandQueueCollection::ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		HRESULT hresult;
		hresult = commandList->Close();
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("Close: ") + errorMsg).c_str());
		}

		ID3D12CommandAllocator* commandAllocator;
		UINT dataSize = sizeof(commandAllocator);
		hresult = commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator);
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("commandList->GetPrivateData: ") + errorMsg).c_str());
		}

		ID3D12CommandList* const ppCommandLists[] = {
			commandList.Get()
		};

		m_commandQueue->ExecuteCommandLists(1, ppCommandLists);
		uint64_t fenceValue = D3DRenderDevice::Signal(m_commandQueue, m_fence, m_fenceValue);

		m_availableCommandAllocatorsQueue.emplace(std::pair<uint64_t, ID3D12CommandAllocator*>(fenceValue, commandAllocator));
		m_availableCommandListsQueue.push(commandList);

		return fenceValue;
	}

	void CommandQueueCollection::WaitForFenceValue(uint64_t fanceValue)
	{
		return D3DRenderDevice::WaitForFenceValue(m_fence, fanceValue, m_fenceEvent);
	}

}
