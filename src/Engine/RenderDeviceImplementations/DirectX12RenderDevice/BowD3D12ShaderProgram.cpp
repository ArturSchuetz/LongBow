#include "BowD3D12ShaderProgram.h"
#include "BowLogger.h"

#include "BowVertexBufferAttribute.h"
#include "BowShaderVertexAttribute.h"

#include "BowD3D12TypeConverter.h"
#include "BowD3D12FragmentOutputs.h"

#include "BowD3D12VertexBuffer.h"
#include "BowD3D12VertexBufferAttributes.h"
#include "BowD3D12VertexArray.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include "d3dx12.h"

namespace bow {

	extern std::string widestring2string(const std::wstring& wstr);
	extern std::wstring string2widestring(const std::string& s);
	extern std::string toErrorString(HRESULT hresult);

	D3DShaderProgram::D3DShaderProgram() : 
		m_rootSignature(nullptr),
		m_pipelineState(nullptr)
	{

	}

	D3DShaderProgram::~D3DShaderProgram()
	{
		Release();
	}

	bool D3DShaderProgram::Initialize(ComPtr<ID3D12Device2> device, const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		m_device = device.Get();

		// set it to true at the beginning and set it to false if an error occured
		m_ready = true;

		HRESULT hresult;
		// Create an empty root signature.
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			hresult = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			if (FAILED(hresult))
			{
				char *errorMessage = reinterpret_cast<char *>(error->GetBufferPointer());
				LOG_ERROR(std::string(std::string("D3D12SerializeRootSignature: ") + errorMessage).c_str());
				m_ready = false;
				return false;
			}

			hresult = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("D3D12SerializeRootSignature: ") + errorMsg).c_str());
				m_ready = false;
				return false;
			}
		}

		// Create the pipeline state, which includes compiling and loading shaders.
		{

#if defined(_DEBUG)
			// Enable better shader debugging with the graphics debugging tools.
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			UINT compileFlags = 0;
#endif

			ComPtr<ID3DBlob> error;
			hresult = D3DCompile(vertexShaderSource.c_str(), vertexShaderSource.size(), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &m_vertexShader, &error);
			if (FAILED(hresult))
			{
				char *errorMessage = reinterpret_cast<char *>(error->GetBufferPointer());
				LOG_ERROR(std::string(std::string("Compiling Vertex Shader:\n") + errorMessage).c_str());
				m_ready = false;
			}
			else
			{
				LOG_TRACE(std::string("Vertex Shader successfully compiled!").c_str());
			}

			hresult = D3DCompile(fragmentShaderSource.c_str(), fragmentShaderSource.size(), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &m_pixelShader, &error);
			if (FAILED(hresult))
			{
				char *errorMessage = reinterpret_cast<char *>(error->GetBufferPointer());
				LOG_ERROR(std::string(std::string("Compiling Pixel Shader:\n") + errorMessage).c_str());
				m_ready = false;
			}
			else
			{
				LOG_TRACE(std::string("Pixel Shader successfully compiled!").c_str());
			}

			m_shaderVertexAttributes = FindVertexAttributes();
			m_fragmentOutputs = FindFragmentOutputs();
		}

		return m_ready;
	}

	void D3DShaderProgram::Release()
	{

	}

	// ===========================================
	// IShaderProgram Functions
	// ===========================================

	ShaderVertexAttributePtr D3DShaderProgram::VGetVertexAttribute(std::string name)
	{
		return m_shaderVertexAttributes[name];
	}

	ShaderVertexAttributeMap D3DShaderProgram::VGetVertexAttributes()
	{
		return m_shaderVertexAttributes;
	}

	int D3DShaderProgram::VGetFragmentOutputLocation(std::string name)
	{
		return -1;
	}

	void D3DShaderProgram::VSetUniform(const char* name, int value)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, float value)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2, int value3)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2, float value3)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2, int value3, int value4)
	{

	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2, float value3, float value4)
	{

	}

	void D3DShaderProgram::VSetUniformVector(const char* name, int* vector, unsigned int vector_size)
	{

	}

	void D3DShaderProgram::VSetUniformVector(const char* name, float* vector, unsigned int vector_size)
	{

	}

	void D3DShaderProgram::VSetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size)
	{

	}

	void D3DShaderProgram::VSetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size)
	{

	}

	// ===========================================
	// ICleanableObserver Functions
	// ===========================================

	void D3DShaderProgram::NotifyDirty(ICleanable* value)
	{

	}

	// ===========================================
	// ===========================================

	ComPtr<ID3D12PipelineState>& D3DShaderProgram::GetPipelineState(VertexArrayPtr vertexArray)
	{
		HRESULT hresult;
		VertexBufferAttributeMap attributeMap = vertexArray->VGetAttributes();

		// Create the vertex input layout
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(attributeMap.size());

		unsigned int layoutElementCount = 0;
		for (auto it = m_shaderVertexAttributes.begin(); it != m_shaderVertexAttributes.end(); it++)
		{
			ShaderVertexAttributePtr shaderVertexAttribute = it->second;
			auto buffer = attributeMap.find(shaderVertexAttribute->Location);
			if (buffer != attributeMap.end())
			{
				VertexBufferAttributePtr attribute = buffer->second;
				D3D12_INPUT_ELEMENT_DESC temp = { shaderVertexAttribute->SemanticName.c_str(), (UINT)shaderVertexAttribute->SemanticIndex, D3DTypeConverter::To(attribute->GetNumberOfComponents(), attribute->GetComponentDatatype()), (UINT)layoutElementCount, layoutElementCount == 0 ? 0 : D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
				inputLayout[layoutElementCount] = temp;
				layoutElementCount++;
			}
		}

		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { &inputLayout[0], layoutElementCount };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		hresult = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("CreateGraphicsPipelineState: ") + errorMsg).c_str());
		}

		return m_pipelineState;
	}

	ComPtr<ID3D12RootSignature>& D3DShaderProgram::GetRootSignature()
	{
		return m_rootSignature;
	}

	// ===========================================
	// Private Functions
	// ===========================================

	ShaderVertexAttributeMap D3DShaderProgram::FindVertexAttributes()
	{
		ComPtr<ID3D12ShaderReflection> vertexShaderReflection;
		D3DReflect(m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize(), IID_PPV_ARGS(vertexShaderReflection.GetAddressOf()));

		D3D12_SHADER_DESC vertexShaderDesc;
		vertexShaderReflection->GetDesc(&vertexShaderDesc);

		ShaderVertexAttributeMap vertexAttributes = ShaderVertexAttributeMap();

		if (vertexShaderDesc.InputParameters > 0)
			LOG_TRACE("\tVertexAttributes:");

		for (unsigned int i = 0; i < (unsigned int)vertexShaderDesc.InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC parameterDesc;
			vertexShaderReflection->GetInputParameterDesc(i, &parameterDesc);

			std::string attributeName = std::string(parameterDesc.SemanticName) + std::to_string(parameterDesc.SemanticIndex);

			LOG_TRACE("\t\tSemanticName: %s, \tRegister: %d", attributeName.c_str(), parameterDesc.Register);

			vertexAttributes.insert(std::pair<std::string, ShaderVertexAttributePtr>(attributeName, ShaderVertexAttributePtr(new ShaderVertexAttribute(parameterDesc.SemanticName, parameterDesc.SemanticIndex, parameterDesc.Register, D3DTypeConverter::ToActiveAttribType(parameterDesc.ComponentType), 1))));
		}

		return vertexAttributes;
	}

	D3DFragmentOutputs D3DShaderProgram::FindFragmentOutputs()
	{
		ComPtr<ID3D12ShaderReflection> pixelShaderReflection;
		D3DReflect(m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize(), IID_PPV_ARGS(pixelShaderReflection.GetAddressOf()));

		D3D12_SHADER_DESC pixelShaderDesc;
		pixelShaderReflection->GetDesc(&pixelShaderDesc);

		if (pixelShaderDesc.OutputParameters > 0)
			LOG_TRACE("\tOutput:");

		for (unsigned int i = 0; i < (unsigned int)pixelShaderDesc.OutputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC parameterDesc;
			pixelShaderReflection->GetOutputParameterDesc(i, &parameterDesc);

			LOG_TRACE("\t\tSemanticName: %s, \tRegister: %d", parameterDesc.SemanticName, parameterDesc.Register);
		}

		return D3DFragmentOutputs();
	}

}
