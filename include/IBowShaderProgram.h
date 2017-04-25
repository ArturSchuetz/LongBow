#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowMath.h"

namespace Bow {
	namespace Renderer{

		class IShaderProgram
		{
		public:
			virtual ~IShaderProgram(){}

			virtual std::string					VGetLog() = 0;
			virtual ShaderVertexAttributePtr	VGetVertexAttribute(std::string name) = 0;
			virtual ShaderVertexAttributeMap	VGetVertexAttributes() = 0;
			virtual int							VGetFragmentOutputLocation(std::string name) = 0;

			void VSetUniform(const char* name, Core::Vector2<int> value) {
				VSetUniformVector(name, value.a, 2);
			}
			void VSetUniform(const char* name, Core::Vector2<float> value) {
				VSetUniformVector(name, value.a, 2);
			}

			void VSetUniform(const char* name, Core::Vector3<int> value) {
				VSetUniformVector(name, value.a, 3);
			}
			void VSetUniform(const char* name, Core::Vector3<float> value) {
				VSetUniformVector(name, value.a, 3);
			}

			void VSetUniform(const char* name, Core::Vector4<int> value) {
				VSetUniformVector(name, value.a, 4);
			}
			void VSetUniform(const char* name, Core::Vector4<float> value) {
				VSetUniformVector(name, value.a, 4);
			}

			void VSetUniform(const char* name, Core::Matrix2D<int> value) {
				VSetUniformMatrix(name, value.a, 9);
			}
			void VSetUniform(const char* name, Core::Matrix2D<float> value) {
				VSetUniformMatrix(name, value.a, 9);
			}

			void VSetUniform(const char* name, Core::Matrix2x2<int> value) {
				VSetUniformMatrix(name, value.a, 4);
			}
			void VSetUniform(const char* name, Core::Matrix2x2<float> value) {
				VSetUniformMatrix(name, value.a, 4);
			}

			void VSetUniform(const char* name, Core::Matrix3D<int> value) {
				VSetUniformMatrix(name, value.a, 16);
			}
			void VSetUniform(const char* name, Core::Matrix3D<float> value) {
				VSetUniformMatrix(name, value.a, 16);
			}

			void VSetUniform(const char* name, Core::Matrix4x4<int> value) {
				VSetUniformMatrix(name, value.a, 16);
			}
			void VSetUniform(const char* name, Core::Matrix4x4<float> value) {
				VSetUniformMatrix(name, value.a, 16);
			}

			virtual void VSetUniform(const char* name, int value) = 0;
			virtual void VSetUniform(const char* name, float value) = 0;

			virtual void VSetUniform(const char* name, int value1, int value2) = 0;
			virtual void VSetUniform(const char* name, float value1, float value2) = 0;

			virtual void VSetUniform(const char* name, int value1, int value2, int value3) = 0;
			virtual void VSetUniform(const char* name, float value1, float value2, float value3) = 0;

			virtual void VSetUniform(const char* name, int value1, int value2, int value3, int value4) = 0;
			virtual void VSetUniform(const char* name, float value1, float value2, float value3, float value4) = 0;

			virtual void VSetUniformVector(const char* name, int* vector, unsigned int vector_size) = 0;
			virtual void VSetUniformVector(const char* name, float* vector, unsigned int vector_size) = 0;

			virtual void VSetUniformMatrix(const char* name, int* matrix, unsigned int matrix_size) = 0;
			virtual void VSetUniformMatrix(const char* name, float* matrix, unsigned int matrix_size) = 0;
		};

	}
}