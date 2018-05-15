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

			FindRootSignature();
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
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value, sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, float value)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value, sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(int));
		memcpy(buffer+ sizeof(int), &value2, sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(float));
		memcpy(buffer + sizeof(float), &value2, sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2, int value3)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(int));
		memcpy(buffer + sizeof(int), &value2, sizeof(int));
		memcpy(buffer + (sizeof(int) * 2), &value3, sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2, float value3)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(float));
		memcpy(buffer + sizeof(float), &value2, sizeof(float));
		memcpy(buffer + (sizeof(float) * 2), &value3, sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, int value1, int value2, int value3, int value4)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(int));
		memcpy(buffer + sizeof(int), &value2, sizeof(int));
		memcpy(buffer + (sizeof(int) * 2), &value3, sizeof(int));
		memcpy(buffer + (sizeof(int) * 3), &value4, sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniform(const char* name, float value1, float value2, float value3, float value4)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, &value1, sizeof(float));
		memcpy(buffer + sizeof(float), &value2, sizeof(float));
		memcpy(buffer + (sizeof(float) * 2), &value3, sizeof(float));
		memcpy(buffer + (sizeof(float) * 3), &value4, sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniformVector(const char* name, int* vector, unsigned int vector_size)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, vector, vector_size * sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniformVector(const char* name, float* vector, unsigned int vector_size)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, vector, vector_size * sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, matrix, matrix_size * sizeof(int));
		m_buffersByVariables[name]->SetDirty();
	}

	void D3DShaderProgram::VSetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size)
	{
		if (m_variables.find(name) == m_variables.end())
		{
			LOG_ERROR(std::string(std::string("Variable with name '") + std::string(name) + std::string("' could not be found")).c_str());
		}
		char* buffer = m_variables[name].first;
		unsigned int size = m_variables[name].second;

		memcpy(buffer, matrix, matrix_size * sizeof(float));
		m_buffersByVariables[name]->SetDirty();
	}

	// ===========================================
	// ICleanableObserver Functions
	// ===========================================

	void D3DShaderProgram::NotifyDirty(ICleanable* value)
	{

	}

	void D3DShaderProgram::BindBuffers(ComPtr<ID3D12GraphicsCommandList> commandList)
	{
		for (unsigned int i = 0; i < m_constantBuffers.size(); i++)
		{
			m_constantBuffers[i]->BindBuffers(commandList);
		}
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
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
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

	void D3DShaderProgram::FindRootSignature()
	{
		HRESULT hresult;
		ID3D12ShaderReflection*					shaderReflection;
		ID3D12ShaderReflectionConstantBuffer*	bufferReflection;
		D3D12_SHADER_INPUT_BIND_DESC			inputBindDescriptor;
		D3D12_SHADER_BUFFER_DESC				bufferDescriptor;
		D3D12_SHADER_VARIABLE_DESC				variableDescriptor;

		hresult = D3DReflect(m_vertexShader->GetBufferPointer(), m_vertexShader->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("D3DReflect: ") + errorMsg).c_str());
			return;
		}

		LOG_TRACE("\tVertexShader Bindings:");
		bool validBinding = true;
		for (unsigned int i = 0; validBinding; i++)
		{
			hresult = shaderReflection->GetResourceBindingDesc(i, &inputBindDescriptor);
			if (FAILED(hresult))
			{
				validBinding = false;
				continue;
			}

			if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
			{
				bufferReflection = shaderReflection->GetConstantBufferByName(inputBindDescriptor.Name);
				hresult = bufferReflection->GetDesc(&bufferDescriptor);
				if (SUCCEEDED(hresult))
				{
					std::shared_ptr<D3DConstantBuffer> buffer = std::shared_ptr<D3DConstantBuffer>(new D3DConstantBuffer(m_device, bufferDescriptor.Size, inputBindDescriptor.BindPoint));
					buffer->Initialize();

					UINT offset = 0;

					LOG_TRACE("\t\t%s (CBUFFER, %u bytes, bindPoint: b%u)", bufferDescriptor.Name, bufferDescriptor.Size, inputBindDescriptor.BindPoint);
					for (unsigned int j = 0; j < bufferDescriptor.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* bufferVariable = bufferReflection->GetVariableByIndex(j);
						bufferVariable->GetDesc(&variableDescriptor);

						LOG_TRACE("\t\t\t%s (%u bytes)", variableDescriptor.Name, variableDescriptor.Size);

						char* pointer = buffer->SetVariableDeclaration(variableDescriptor.Name, offset, variableDescriptor.Size);
						offset += variableDescriptor.Size;

						m_variables.insert(std::pair<std::string, std::pair<char*, UINT>>(variableDescriptor.Name, std::pair<char*, UINT>(pointer, variableDescriptor.Size)));
						m_buffersByVariables.insert(std::pair<std::string, std::shared_ptr<D3DConstantBuffer>>(variableDescriptor.Name, buffer));
					}

					m_constantBuffers.push_back(buffer);
				}
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER)
			{
				LOG_TRACE("\t\t%s (TBUFFER)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
			{
				LOG_TRACE("\t\t%s (TEXTURE, bindPoint: t%u)", inputBindDescriptor.Name, inputBindDescriptor.BindPoint);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
			{
				LOG_TRACE("\t\t%s (SAMPLER, bindPoint: s%u)", inputBindDescriptor.Name, inputBindDescriptor.BindPoint);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
			{
				LOG_TRACE("\t\t%s (UAV_RWTYPED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_RWSTRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS)
			{
				LOG_TRACE("\t\t%s (BYTEADDRESS)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS)
			{
				LOG_TRACE("\t\t%s (UAV_RWBYTEADDRESS)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_APPEND_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_APPEND_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_CONSUME_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER)
			{
				LOG_TRACE("\t\t%s (UAV_RWSTRUCTURED_WITH_COUNTER)", inputBindDescriptor.Name);
			}
		}

		hresult = D3DReflect(m_pixelShader->GetBufferPointer(), m_pixelShader->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
		if (FAILED(hresult))
		{
			std::string errorMsg = toErrorString(hresult);
			LOG_ERROR(std::string(std::string("D3DReflect: ") + errorMsg).c_str());
			return;
		}

		LOG_TRACE("\tPixelShader Bindings:");
		validBinding = true;
		for (unsigned int i = 0; validBinding; i++)
		{
			hresult = shaderReflection->GetResourceBindingDesc(i, &inputBindDescriptor);
			if (FAILED(hresult))
			{
				validBinding = false;
				continue;
			}

			if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
			{
				bufferReflection = shaderReflection->GetConstantBufferByName(inputBindDescriptor.Name);
				hresult = bufferReflection->GetDesc(&bufferDescriptor);
				if (SUCCEEDED(hresult))
				{
					LOG_TRACE("\t\t%s (CBUFFER, %u bytes)", bufferDescriptor.Name, bufferDescriptor.Size);
					for (unsigned int j = 0; j < bufferDescriptor.Variables; j++)
					{
						ID3D12ShaderReflectionVariable* bufferVariable = bufferReflection->GetVariableByIndex(j);
						bufferVariable->GetDesc(&variableDescriptor);

						LOG_TRACE("\t\t\t%s (%u bytes)", variableDescriptor.Name, variableDescriptor.Size);
					}
				}
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER)
			{
				LOG_TRACE("\t\t%s (TBUFFER)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
			{
				LOG_TRACE("\t\t%s (TEXTURE, bindPoint: t%u)", inputBindDescriptor.Name, inputBindDescriptor.BindPoint);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
			{
				LOG_TRACE("\t\t%s (SAMPLER, bindPoint: s%u)", inputBindDescriptor.Name, inputBindDescriptor.BindPoint);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
			{
				LOG_TRACE("\t\t%s (UAV_RWTYPED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_RWSTRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS)
			{
				LOG_TRACE("\t\t%s (BYTEADDRESS)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS)
			{
				LOG_TRACE("\t\t%s (UAV_RWBYTEADDRESS)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_APPEND_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_APPEND_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED)
			{
				LOG_TRACE("\t\t%s (UAV_CONSUME_STRUCTURED)", inputBindDescriptor.Name);
			}
			else if (inputBindDescriptor.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER)
			{
				LOG_TRACE("\t\t%s (UAV_RWSTRUCTURED_WITH_COUNTER)", inputBindDescriptor.Name);
			}
		}

		// Create the root signature.
		{
			CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
			CD3DX12_ROOT_PARAMETER1 rootParameters[1];

			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
			rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);


			// Allow input layout and deny uneccessary access to certain pipeline stages.
			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);


			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}


			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			hresult = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("D3DX12SerializeVersionedRootSignature: ") + errorMsg).c_str());
				m_ready = false;
				return;
			}


			hresult = m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
			if (FAILED(hresult))
			{
				std::string errorMsg = toErrorString(hresult);
				LOG_ERROR(std::string(std::string("CreateRootSignature: ") + errorMsg).c_str());
				m_ready = false;
				return;
			}
		}
	}

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
