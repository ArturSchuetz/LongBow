#pragma once
#include "BowPrerequisites.h"

namespace Bow {
	namespace Renderer{

		class IShaderProgram
		{
		public:
			virtual ~IShaderProgram(){}

			virtual std::string					GetLog() = 0;
			virtual ShaderVertexAttributePtr	GetVertexAttribute(std::string name) = 0;
			virtual int							GetFragmentOutputLocation(std::string name) = 0;

			virtual void SetUniform(const char* name, int value) = 0;
			virtual void SetUniform(const char* name, float value) = 0;

			virtual void SetUniform(const char* name, int value1, int value2) = 0;
			virtual void SetUniform(const char* name, float value1, float value2) = 0;

			virtual void SetUniform(const char* name, int value1, int value2, int value3) = 0;
			virtual void SetUniform(const char* name, float value1, float value2, float value3) = 0;

			virtual void SetUniform(const char* name, int value1, int value2, int value3, int value4) = 0;
			virtual void SetUniform(const char* name, float value1, float value2, float value3, float value4) = 0;

			virtual void SetUniformVector(const char* name, int* vector, unsigned int vector_size) = 0;
			virtual void SetUniformVector(const char* name, float* vector, unsigned int vector_size) = 0;

			virtual void SetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size) = 0;
			virtual void SetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size) = 0;
		};

	}
}