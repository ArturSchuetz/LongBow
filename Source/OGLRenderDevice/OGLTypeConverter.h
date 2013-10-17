#pragma once
#include "LongBow.h"

#include "BowClearState.h"
#include "BowRenderState.h"
#include "IBowRenderContext.h"
#include "IBowShaderVertexAttribute.h"
#include "IBowUniform.h"

// Buffer
#include "BowComponentDatatype.h"
#include "BowBufferHint.h"

// Texture
#include "IBowTextureSampler.h"
#include "IBowTexture2D.h"

#include <GL/glew.h>

namespace Bow {

class OGLTypeConverter
{
public:
	// ClearState
	static unsigned int					To(ClearBuffers mask);
	static ShaderVertexAttributeType	ToActiveAttribType(GLenum type);
	static UniformType					ToActiveUniformType(GLenum type);

	// Index and Vertex Buffer
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