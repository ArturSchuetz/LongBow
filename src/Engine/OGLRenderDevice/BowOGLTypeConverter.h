#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer {

		typedef unsigned int GLenum;
		typedef unsigned int GLbitfield;
		enum class UniformType : char;

		class OGLTypeConverter
		{
		public:
			// ClearState
			static GLbitfield					To(ClearBuffers mask);
			static ShaderVertexAttributeType	ToActiveAttribType(GLenum type);
			static UniformType					ToActiveUniformType(GLenum type);

			// Index and Vertex Buffer
			static GLenum						To(IndexBufferDatatype type);
			static BufferHint					ToBufferHint(GLenum hint);
			static GLenum						To(BufferHint hint);
			static GLenum						To(ComponentDatatype type);
			static GLenum						To(PrimitiveType type);

			// RenderState
			static GLenum						To(BlendEquation equation);
			static GLenum						To(SourceBlendingFactor factor);
			static GLenum						To(DestinationBlendingFactor factor);
			static GLenum						To(DepthTestFunction function);
			static GLenum						To(CullFace face);
			static GLenum						To(WindingOrder windingOrder);
			static GLenum						To(RasterizationMode mode);
			static GLenum						To(StencilOperation operation);
			static GLenum						To(StencilTestFunction function);

			// Texture
			static GLenum						To(TextureFormat format);
			static GLenum						To(ImageFormat format);
			static GLenum						To(ImageDatatype type);
			static GLenum						TextureToPixelFormat(TextureFormat textureFormat);
			static GLenum						TextureToPixelType(TextureFormat textureFormat);
			static bool							IsTextureFormatValid(TextureFormat textureFormat);
			static GLenum						To(TextureMinificationFilter filter);
			static GLenum						To(TextureMagnificationFilter filter);
			static GLenum						To(TextureWrap wrap);
		};

	}
}