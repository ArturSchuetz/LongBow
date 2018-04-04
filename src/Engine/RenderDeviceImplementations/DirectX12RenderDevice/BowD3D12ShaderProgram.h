#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowShaderProgram.h"
#include "IBowCleanableObserver.h"

#include "BowD3D12FragmentOutputs.h"

#include <d3d12.h>
#include <dxgi1_6.h>

namespace bow {

	class D3DShaderProgram : public IShaderProgram, public ICleanableObserver
	{
		enum class PrimitiveDatatype : char
		{
			Int = 0,
			Float,
			Uint
		};

	public:
		D3DShaderProgram();
		~D3DShaderProgram();

		bool Initialize(ComPtr<ID3D12Device2> device, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
		void Release();

		// ===========================================
		// IShaderProgram Functions
		// ===========================================

		ShaderVertexAttributePtr	VGetVertexAttribute(std::string name);
		ShaderVertexAttributeMap	VGetVertexAttributes();
		int							VGetFragmentOutputLocation(std::string name);

		void VSetUniform(const char* name, int value);
		void VSetUniform(const char* name, float value);

		void VSetUniform(const char* name, int value1, int value2);
		void VSetUniform(const char* name, float value1, float value2);

		void VSetUniform(const char* name, int value1, int value2, int value3);
		void VSetUniform(const char* name, float value1, float value2, float value3);

		void VSetUniform(const char* name, int value1, int value2, int value3, int value4);
		void VSetUniform(const char* name, float value1, float value2, float value3, float value4);

		void VSetUniformVector(const char* name, int* vector, unsigned int vector_size);
		void VSetUniformVector(const char* name, float* vector, unsigned int vector_size);

		void VSetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size);
		void VSetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size);

		// ===========================================
		// ICleanableObserver Functions
		// ===========================================

		void NotifyDirty(ICleanable* value);

		// ===========================================

		ComPtr<ID3D12PipelineState>& GetPipelineState(VertexArrayPtr vertexArray);
		ComPtr<ID3D12RootSignature>& GetRootSignature();

	private:
		D3DShaderProgram(D3DShaderProgram&) {}
		D3DShaderProgram& operator=(const D3DShaderProgram&) { return *this; }

		ShaderVertexAttributeMap FindVertexAttributes();
		D3DFragmentOutputs FindFragmentOutputs();

		ComPtr<ID3DBlob> m_vertexShader;
		ComPtr<ID3DBlob> m_pixelShader;

		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;

		ShaderVertexAttributeMap m_shaderVertexAttributes;
		D3DFragmentOutputs m_fragmentOutputs;

		ID3D12Device2* m_device;
		bool m_ready; 
	};

}
