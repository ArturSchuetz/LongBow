#include "BowOGLTypeConverter.h"
#include "BowLogger.h"

// Uniform
#include "BowOGLUniform.h"

// 
#include "IBowRenderContext.h"
#include "BowClearState.h"
#include "BowRenderState.h"
#include "BowShaderVertexAttribute.h"

// Buffer
#include "BowIndexBufferDatatype.h"
#include "BowComponentDatatype.h"
#include "BowBufferHint.h"

// Texture
#include "IBowTextureSampler.h"
#include "IBowTexture2D.h"

#include <GL/glew.h>

namespace Bow{
	namespace Renderer{

		GLbitfield OGLTypeConverter::To(ClearBuffers mask)
		{
			GLbitfield clearMask = 0;

			if ((mask & ClearBuffers::ColorBuffer) != 0)
			{
				clearMask |= GL_COLOR_BUFFER_BIT;
			}

			if ((mask & ClearBuffers::DepthBuffer) != 0)
			{
				clearMask |= GL_DEPTH_BUFFER_BIT;
			}

			if ((mask & ClearBuffers::StencilBuffer) != 0)
			{
				clearMask |= GL_STENCIL_BUFFER_BIT;
			}
			return clearMask;
		}


		ShaderVertexAttributeType OGLTypeConverter::ToActiveAttribType(GLenum type)
		{
			switch (type)
			{
				case GL_FLOAT:
					return ShaderVertexAttributeType::Float;
				case GL_FLOAT_VEC2:
					return ShaderVertexAttributeType::FloatVector2;
				case GL_FLOAT_VEC3:
					return ShaderVertexAttributeType::FloatVector3;
				case GL_FLOAT_VEC4:
					return ShaderVertexAttributeType::FloatVector4;
				case GL_FLOAT_MAT2:
					return ShaderVertexAttributeType::FloatMatrix22;
				case GL_FLOAT_MAT3:
					return ShaderVertexAttributeType::FloatMatrix33;
				case GL_FLOAT_MAT4:
					return ShaderVertexAttributeType::FloatMatrix44;
				case GL_INT:
					return ShaderVertexAttributeType::Int;
				case GL_INT_VEC2:
					return ShaderVertexAttributeType::IntVector2;
				case GL_INT_VEC3:
					return ShaderVertexAttributeType::IntVector3;
				case GL_INT_VEC4:
					return ShaderVertexAttributeType::IntVector4;
			}

			LOG_FATAL("ActiveAttribType does not exist.");
			return ShaderVertexAttributeType::INVALID;
		}


		UniformType OGLTypeConverter::ToActiveUniformType(GLenum type)
		{
			switch (type)
			{
				case GL_INT:
					return UniformType::Int;
				case GL_FLOAT:
					return UniformType::Float;
				case GL_FLOAT_VEC2:
					return UniformType::FloatVector2;
				case GL_FLOAT_VEC3:
					return UniformType::FloatVector3;
				case GL_FLOAT_VEC4:
					return UniformType::FloatVector4;
				case GL_INT_VEC2:
					return UniformType::IntVector2;
				case GL_INT_VEC3:
					return UniformType::IntVector3;
				case GL_INT_VEC4:
					return UniformType::IntVector4;
				case GL_BOOL:
					return UniformType::Bool;
				case GL_BOOL_VEC2:
					return UniformType::BoolVector2;
				case GL_BOOL_VEC3:
					return UniformType::BoolVector3;
				case GL_BOOL_VEC4:
					return UniformType::BoolVector4;
				case GL_FLOAT_MAT2:
					return UniformType::FloatMatrix22;
				case GL_FLOAT_MAT3:
					return UniformType::FloatMatrix33;
				case GL_FLOAT_MAT4:
					return UniformType::FloatMatrix44;
				case GL_SAMPLER_1D:
					return UniformType::Sampler1D;
				case GL_SAMPLER_2D:
					return UniformType::Sampler2D;
				case GL_SAMPLER_2D_RECT:
					return UniformType::Sampler2DRectangle;
				case GL_SAMPLER_2D_RECT_SHADOW:
					return UniformType::Sampler2DRectangleShadow;
				case GL_SAMPLER_3D:
					return UniformType::Sampler3D;
				case GL_SAMPLER_CUBE:
					return UniformType::SamplerCube;
				case GL_SAMPLER_1D_SHADOW:
					return UniformType::Sampler1DShadow;
				case GL_SAMPLER_2D_SHADOW:
					return UniformType::Sampler2DShadow;
				case GL_FLOAT_MAT2x3:
					return UniformType::FloatMatrix23;
				case GL_FLOAT_MAT2x4:
					return UniformType::FloatMatrix24;
				case GL_FLOAT_MAT3x2:
					return UniformType::FloatMatrix32;
				case GL_FLOAT_MAT3x4:
					return UniformType::FloatMatrix34;
				case GL_FLOAT_MAT4x2:
					return UniformType::FloatMatrix42;
				case GL_FLOAT_MAT4x3:
					return UniformType::FloatMatrix43;
				case GL_SAMPLER_1D_ARRAY:
					return UniformType::Sampler1DArray;
				case GL_SAMPLER_2D_ARRAY:
					return UniformType::Sampler2DArray;
				case GL_SAMPLER_1D_ARRAY_SHADOW:
					return UniformType::Sampler1DArrayShadow;
				case GL_SAMPLER_2D_ARRAY_SHADOW:
					return UniformType::Sampler2DArrayShadow;
				case GL_SAMPLER_CUBE_SHADOW:
					return UniformType::SamplerCubeShadow;
				case GL_INT_SAMPLER_1D:
					return UniformType::IntSampler1D;
				case GL_INT_SAMPLER_2D:
					return UniformType::IntSampler2D;
				case GL_INT_SAMPLER_2D_RECT:
					return UniformType::IntSampler2DRectangle;
				case GL_INT_SAMPLER_3D:
					return UniformType::IntSampler3D;
				case GL_INT_SAMPLER_CUBE:
					return UniformType::IntSamplerCube;
				case GL_INT_SAMPLER_1D_ARRAY:
					return UniformType::IntSampler1DArray;
				case GL_INT_SAMPLER_2D_ARRAY:
					return UniformType::IntSampler2DArray;
				case GL_UNSIGNED_INT_SAMPLER_1D:
					return UniformType::UnsignedIntSampler1D;
				case GL_UNSIGNED_INT_SAMPLER_2D:
					return UniformType::UnsignedIntSampler2D;
				case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
					return UniformType::UnsignedIntSampler2DRectangle;
				case GL_UNSIGNED_INT_SAMPLER_3D:
					return UniformType::UnsignedIntSampler3D;
				case GL_UNSIGNED_INT_SAMPLER_CUBE:
					return UniformType::UnsignedIntSamplerCube;
				case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
					return UniformType::UnsignedIntSampler1DArray;
				case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
					return UniformType::UnsignedIntSampler2DArray;
			}

			LOG_FATAL("ActiveAttribType does not exist.");
			return UniformType::INVALID;
		}

		// Index and Vertex Buffer

		GLenum OGLTypeConverter::To(IndexBufferDatatype type)
		{
			switch (type)
			{
			case IndexBufferDatatype::UnsignedShort:
				return GL_UNSIGNED_SHORT;
			case IndexBufferDatatype::UnsignedInt:
				return GL_UNSIGNED_INT;
			}

			LOG_FATAL("IndexBufferDatatype does not exist.");
			return GL_INVALID_ENUM;
		}

		BufferHint OGLTypeConverter::ToBufferHint(GLenum hint)
		{
			switch (hint)
			{
				case GL_STREAM_DRAW:
					return BufferHint::StreamDraw;
				case GL_STREAM_READ:
					return BufferHint::StreamRead;
				case GL_STREAM_COPY:
					return BufferHint::StreamCopy;
				case GL_STATIC_DRAW:
					return BufferHint::StaticDraw;
				case GL_STATIC_READ:
					return BufferHint::StaticRead;
				case GL_STATIC_COPY:
					return BufferHint::StaticCopy;
				case GL_DYNAMIC_DRAW:
					return BufferHint::DynamicDraw;
				case GL_DYNAMIC_READ:
					return BufferHint::DynamicRead;
				case GL_DYNAMIC_COPY:
					return BufferHint::DynamicCopy;
			}

			LOG_FATAL("BufferUsageHint does not exist.");
			return BufferHint::None;
		}

		GLenum OGLTypeConverter::To(BufferHint hint)
		{
			switch (hint)
			{
				case BufferHint::StreamDraw:
					return GL_STREAM_DRAW;
				case BufferHint::StreamRead:
					return GL_STREAM_READ;
				case BufferHint::StreamCopy:
					return GL_STREAM_COPY;
				case BufferHint::StaticDraw:
					return GL_STATIC_DRAW;
				case BufferHint::StaticRead:
					return GL_STATIC_READ;
				case BufferHint::StaticCopy:
					return GL_STATIC_COPY;
				case BufferHint::DynamicDraw:
					return GL_DYNAMIC_DRAW;
				case BufferHint::DynamicRead:
					return GL_DYNAMIC_READ;
				case BufferHint::DynamicCopy:
					return GL_DYNAMIC_COPY;
			}

			LOG_FATAL("BufferHint does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(ComponentDatatype type)
		{
			switch (type)
			{
				case ComponentDatatype::Byte:
					return GL_BYTE;
				case ComponentDatatype::UnsignedByte:
					return GL_UNSIGNED_BYTE;
				case ComponentDatatype::Short:
					return GL_SHORT;
				case ComponentDatatype::UnsignedShort:
					return GL_UNSIGNED_SHORT;
				case ComponentDatatype::Int:
					return GL_INT;
				case ComponentDatatype::UnsignedInt:
					return GL_UNSIGNED_INT;
				case ComponentDatatype::Float:
					return GL_FLOAT;
					/*
				case ComponentDatatype::HalfFloat:
					return GL_HALF_FLOAT;
				case ComponentDatatype::Double:
					return GL_DOUBLE;
					*/
			}

			LOG_FATAL("ComponentDatatype does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::To(PrimitiveType type)
		{
			switch (type)
			{
				case PrimitiveType::Points:
					return GL_POINTS;
				case PrimitiveType::Lines:
					return GL_LINES;
				case PrimitiveType::LineLoop:
					return GL_LINE_LOOP;
				case PrimitiveType::LineStrip:
					return GL_LINE_STRIP;
				case PrimitiveType::Triangles:
					return GL_TRIANGLES;
				case PrimitiveType::TriangleStrip:
					return GL_TRIANGLE_STRIP;
				case PrimitiveType::LinesAdjacency:
					return GL_LINES_ADJACENCY;
				case PrimitiveType::LineStripAdjacency:
					return GL_LINE_STRIP_ADJACENCY;
				case PrimitiveType::TrianglesAdjacency:
					return GL_TRIANGLES_ADJACENCY;
				case PrimitiveType::TriangleStripAdjacency:
					return GL_TRIANGLE_STRIP_ADJACENCY;
				case PrimitiveType::TriangleFan:
					return GL_TRIANGLE_FAN;

				//ToDo: GL_PATCHES
			}

			LOG_FATAL("PrimitiveType does not exist.");
			return GL_INVALID_ENUM;
		}

		// Renderstates

		GLenum OGLTypeConverter::To(DepthTestFunction function)
		{
			switch (function)
			{
				case DepthTestFunction::Never:
					return GL_NEVER;
				case DepthTestFunction::Less:
					return GL_LESS;
				case DepthTestFunction::Equal:
					return GL_EQUAL;
				case DepthTestFunction::LessThanOrEqual:
					return GL_LEQUAL;
				case DepthTestFunction::Greater:
					return GL_GREATER;
				case DepthTestFunction::NotEqual:
					return GL_NOTEQUAL;
				case DepthTestFunction::GreaterThanOrEqual:
					return GL_GEQUAL;
				case DepthTestFunction::Always:
					return GL_ALWAYS;
			}

			LOG_FATAL("DepthTestFunction does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(CullFace face)
		{
			switch (face)
			{
				case CullFace::Front:
					return GL_FRONT;
				case CullFace::Back:
					return GL_BACK;
				case CullFace::FrontAndBack:
					return GL_FRONT_AND_BACK;
			}

			LOG_FATAL("CullFace does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(WindingOrder windingOrder)
		{
			switch (windingOrder)
			{
				case WindingOrder::Clockwise:
					return GL_CW;
				case WindingOrder::Counterclockwise:
					return GL_CCW;
			}

			LOG_FATAL("WindingOrder does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(RasterizationMode mode)
		{
			switch (mode)
			{
				case RasterizationMode::Point:
					return GL_POINT;
				case RasterizationMode::Line:
					return GL_LINE;
				case RasterizationMode::Fill:
					return GL_FILL;
			}

			LOG_FATAL("RasterizationMode does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(StencilOperation operation)
		{
			switch (operation)
			{
				case StencilOperation::Zero:
					return GL_ZERO;
				case StencilOperation::Invert:
					return GL_INVERT;
				case StencilOperation::Keep:
					return GL_KEEP;
				case StencilOperation::Replace:
					return GL_REPLACE;
				case StencilOperation::Increment:
					return GL_INCR;
				case StencilOperation::Decrement:
					return GL_DECR;
				case StencilOperation::IncrementWrap:
					return GL_INCR_WRAP;
				case StencilOperation::DecrementWrap:
					return GL_DECR_WRAP;
			}

			LOG_FATAL("StencilOperation does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(StencilTestFunction function)
		{
			switch (function)
			{
				case StencilTestFunction::Never:
					return GL_NEVER;
				case StencilTestFunction::Less:
					return GL_LESS;
				case StencilTestFunction::Equal:
					return GL_EQUAL;
				case StencilTestFunction::LessThanOrEqual:
					return GL_LEQUAL;
				case StencilTestFunction::Greater:
					return GL_GREATER;
				case StencilTestFunction::NotEqual:
					return GL_NOTEQUAL;
				case StencilTestFunction::GreaterThanOrEqual:
					return GL_GEQUAL;
				case StencilTestFunction::Always:
					return GL_ALWAYS;
			}

			LOG_FATAL("StencilTestFunction does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(BlendEquation equation)
		{
			switch (equation)
			{
			case BlendEquation::Add:
					return GL_FUNC_ADD;
			case BlendEquation::Minimum:
					return GL_MIN;
			case BlendEquation::Maximum:
					return GL_MAX;
			case BlendEquation::Subtract:
					return GL_FUNC_SUBTRACT;
			case BlendEquation::ReverseSubtract:
					return GL_FUNC_REVERSE_SUBTRACT;
			}

			LOG_FATAL("BlendEquation does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(SourceBlendingFactor factor)
		{
			switch (factor)
			{
				case SourceBlendingFactor::Zero:
					return GL_ZERO;
				case SourceBlendingFactor::One:
					return GL_ONE;
				case SourceBlendingFactor::SourceAlpha:
					return GL_SRC_ALPHA;
				case SourceBlendingFactor::OneMinusSourceAlpha:
					return GL_ONE_MINUS_SRC_ALPHA;
				case SourceBlendingFactor::DestinationAlpha:
					return GL_DST_ALPHA;
				case SourceBlendingFactor::OneMinusDestinationAlpha:
					return GL_ONE_MINUS_DST_ALPHA;
				case SourceBlendingFactor::DestinationColor:
					return GL_DST_COLOR;
				case SourceBlendingFactor::OneMinusDestinationColor:
					return GL_ONE_MINUS_DST_COLOR;
				case SourceBlendingFactor::SourceAlphaSaturate:
					return GL_SRC_ALPHA_SATURATE;
				case SourceBlendingFactor::ConstantColor:
					return GL_CONSTANT_COLOR;
				case SourceBlendingFactor::OneMinusConstantColor:
					return GL_ONE_MINUS_CONSTANT_COLOR;
				case SourceBlendingFactor::ConstantAlpha:
					return GL_CONSTANT_ALPHA;
				case SourceBlendingFactor::OneMinusConstantAlpha:
					return GL_ONE_MINUS_CONSTANT_ALPHA;
			}

			LOG_FATAL("SourceBlendingFactor does not exist.");
			return GL_INVALID_ENUM;
		}


		GLenum OGLTypeConverter::To(DestinationBlendingFactor factor)
		{
			switch (factor)
			{
				case DestinationBlendingFactor::Zero:
					return GL_ZERO;
				case DestinationBlendingFactor::One:
					return GL_ONE;
				case DestinationBlendingFactor::SourceColor:
					return GL_SRC_COLOR;
				case DestinationBlendingFactor::OneMinusSourceColor:
					return GL_ONE_MINUS_SRC_COLOR;
				case DestinationBlendingFactor::SourceAlpha:
					return GL_SRC_ALPHA;
				case DestinationBlendingFactor::OneMinusSourceAlpha:
					return GL_ONE_MINUS_SRC_ALPHA;
				case DestinationBlendingFactor::DestinationAlpha:
					return GL_DST_ALPHA;
				case DestinationBlendingFactor::OneMinusDestinationAlpha:
					return GL_ONE_MINUS_DST_ALPHA;
				case DestinationBlendingFactor::DestinationColor:
					return GL_DST_COLOR;
				case DestinationBlendingFactor::OneMinusDestinationColor:
					return GL_ONE_MINUS_DST_COLOR;
				case DestinationBlendingFactor::ConstantColor:
					return GL_CONSTANT_COLOR;
				case DestinationBlendingFactor::OneMinusConstantColor:
					return GL_ONE_MINUS_CONSTANT_COLOR;
				case DestinationBlendingFactor::ConstantAlpha:
					return GL_CONSTANT_ALPHA;
				case DestinationBlendingFactor::OneMinusConstantAlpha:
					return GL_ONE_MINUS_CONSTANT_ALPHA;
			}

			LOG_FATAL("DestinationBlendingFactor does not exist.");
			return GL_INVALID_ENUM;
		}

		// Texture
		GLenum OGLTypeConverter::To(TextureFormat format)
		{
			switch (format)
			{
				case TextureFormat::RedGreenBlue8:
					return GL_RGB8;
				case TextureFormat::RedGreenBlue16:
					return GL_RGB16;
				case TextureFormat::RedGreenBlueAlpha8:
					return GL_RGBA8;
				case TextureFormat::RedGreenBlue10A2:
					return GL_RGB10_A2;
				case TextureFormat::RedGreenBlueAlpha16:
					return GL_RGBA16;
				case TextureFormat::Depth16:
					return GL_DEPTH_COMPONENT16;
				case TextureFormat::Depth24:
					return GL_DEPTH_COMPONENT24;
				case TextureFormat::Red8:
					return GL_R8;
				case TextureFormat::Red16:
					return GL_R16;
				case TextureFormat::RedGreen8:
					return GL_RG8;
				case TextureFormat::RedGreen16:
					return GL_RG16;
				case TextureFormat::Red16f:
					return GL_R16F;
				case TextureFormat::Red32f:
					return GL_R32F;
				case TextureFormat::RedGreen16f:
					return GL_RG16F;
				case TextureFormat::RedGreen32f:
					return GL_R32F;
				case TextureFormat::Red8i:
					return GL_R8I;
				case TextureFormat::Red8ui:
					return GL_R8UI;
				case TextureFormat::Red16i:
					return GL_R16I;
				case TextureFormat::Red16ui:
					return GL_R16UI;
				case TextureFormat::Red32i:
					return GL_R32I;
				case TextureFormat::Red32ui:
					return GL_R32UI;
				case TextureFormat::RedGreen8i:
					return GL_RG8I;
				case TextureFormat::RedGreen8ui:
					return GL_RG8UI;
				case TextureFormat::RedGreen16i:
					return GL_RG16I;
				case TextureFormat::RedGreen16ui:
					return GL_RG16UI;
				case TextureFormat::RedGreen32i:
					return GL_RG32I;
				case TextureFormat::RedGreen32ui:
					return GL_RG32UI;
				case TextureFormat::RedGreenBlueAlpha32f:
					return GL_RGBA32F;
				case TextureFormat::RedGreenBlue32f:
					return GL_RGB32F;
				case TextureFormat::RedGreenBlueAlpha16f:
					return GL_RGBA16F;
				case TextureFormat::RedGreenBlue16f:
					return GL_RGB16F;
				case TextureFormat::Depth24Stencil8:
					return GL_DEPTH24_STENCIL8;
				case TextureFormat::Red11fGreen11fBlue10f:
					return GL_R11F_G11F_B10F;
				case TextureFormat::RedGreenBlue9E5:
					return GL_RGB9_E5;
				case TextureFormat::SRedGreenBlue8:
					return GL_SRGB8;
				case TextureFormat::SRedGreenBlue8Alpha8:
					return GL_SRGB8_ALPHA8;
				case TextureFormat::Depth32f:
					return GL_DEPTH_COMPONENT32F;
				case TextureFormat::Depth32fStencil8:
					return GL_DEPTH32F_STENCIL8;
				case TextureFormat::RedGreenBlueAlpha32ui:
					return GL_RGBA32UI;
				case TextureFormat::RedGreenBlue32ui:
					return GL_RGB32UI;
				case TextureFormat::RedGreenBlueAlpha16ui:
					return GL_RGBA16UI;
				case TextureFormat::RedGreenBlue16ui:
					return GL_RGB16UI;
				case TextureFormat::RedGreenBlueAlpha8ui:
					return GL_RGBA8UI;
				case TextureFormat::RedGreenBlue8ui:
					return GL_RGB8UI;
				case TextureFormat::RedGreenBlueAlpha32i:
					return GL_RGBA32I;
				case TextureFormat::RedGreenBlue32i:
					return GL_RGB32I;
				case TextureFormat::RedGreenBlueAlpha16i:
					return GL_RGBA16I;
				case TextureFormat::RedGreenBlue16i:
					return GL_RGB16I;
				case TextureFormat::RedGreenBlueAlpha8i:
					return GL_RGBA8I;
				case TextureFormat::RedGreenBlue8i:
					return GL_RGB8I;
			}

			LOG_FATAL("TextureFormat does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::To(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::StencilIndex:
					return GL_STENCIL_INDEX;
				case ImageFormat::DepthComponent:
					return GL_DEPTH_COMPONENT;
				case ImageFormat::Red:
					return GL_RED;
				case ImageFormat::Green:
					return GL_GREEN;
				case ImageFormat::Blue:
					return GL_BLUE;
				case ImageFormat::RedGreenBlue:
					return GL_RGB;
				case ImageFormat::RedGreenBlueAlpha:
					return GL_RGBA;
				case ImageFormat::BlueGreenRed:
					return GL_BGR;
				case ImageFormat::BlueGreenRedAlpha:
					return GL_BGRA;
				case ImageFormat::RedGreen:
					return GL_RG;
				case ImageFormat::RedGreenInteger:
					return GL_RG_INTEGER;
				case ImageFormat::DepthStencil:
					return GL_DEPTH_STENCIL;
				case ImageFormat::RedInteger:
					return GL_RED_INTEGER;
				case ImageFormat::GreenInteger:
					return GL_GREEN_INTEGER;
				case ImageFormat::BlueInteger:
					return GL_BLUE_INTEGER;
				case ImageFormat::RedGreenBlueInteger:
					return GL_RGB_INTEGER;
				case ImageFormat::RedGreenBlueAlphaInteger:
					return GL_RGBA_INTEGER;
				case ImageFormat::BlueGreenRedInteger:
					return GL_BGR_INTEGER;
				case ImageFormat::BlueGreenRedAlphaInteger:
					return GL_BGRA_INTEGER;
			}

			LOG_FATAL("ImageFormat does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::To(ImageDatatype type)
		{
			switch (type)
			{
				case ImageDatatype::Byte:
					return GL_BYTE;
				case ImageDatatype::UnsignedByte:
					return GL_UNSIGNED_BYTE;
				case ImageDatatype::Short:
					return GL_SHORT;
				case ImageDatatype::UnsignedShort:
					return GL_UNSIGNED_SHORT;
				case ImageDatatype::Int:
					return GL_INT;
				case ImageDatatype::UnsignedInt:
					return GL_UNSIGNED_INT;
				case ImageDatatype::Float:
					return GL_FLOAT;
				case ImageDatatype::HalfFloat:
					return GL_HALF_FLOAT;
				case ImageDatatype::UnsignedByte332:
					return GL_UNSIGNED_BYTE_3_3_2;
				case ImageDatatype::UnsignedShort4444:
					return GL_UNSIGNED_SHORT_4_4_4_4;
				case ImageDatatype::UnsignedShort5551:
					return GL_UNSIGNED_SHORT_5_5_5_1;
				case ImageDatatype::UnsignedInt8888:
					return GL_UNSIGNED_INT_8_8_8_8;
				case ImageDatatype::UnsignedInt1010102:
					return GL_UNSIGNED_INT_10_10_10_2;
				case ImageDatatype::UnsignedByte233Reversed:
					return GL_UNSIGNED_BYTE_2_3_3_REV;
				case ImageDatatype::UnsignedShort565:
					return GL_UNSIGNED_SHORT_5_6_5;
				case ImageDatatype::UnsignedShort565Reversed:
					return GL_UNSIGNED_SHORT_5_6_5_REV;
				case ImageDatatype::UnsignedShort4444Reversed:
					return GL_UNSIGNED_SHORT_4_4_4_4_REV;
				case ImageDatatype::UnsignedShort1555Reversed:
					return GL_UNSIGNED_SHORT_1_5_5_5_REV;
				case ImageDatatype::UnsignedInt8888Reversed:
					return GL_UNSIGNED_INT_8_8_8_8_REV;
				case ImageDatatype::UnsignedInt2101010Reversed:
					return GL_UNSIGNED_INT_2_10_10_10_REV;
				case ImageDatatype::UnsignedInt248:
					return GL_UNSIGNED_INT_24_8;
				case ImageDatatype::UnsignedInt10F11F11FReversed:
					return GL_UNSIGNED_INT_10F_11F_11F_REV;
				case ImageDatatype::UnsignedInt5999Reversed:
					return GL_UNSIGNED_INT_5_9_9_9_REV;
				case ImageDatatype::Float32UnsignedInt248Reversed:
					return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			}

			LOG_FATAL("ImageDatatype does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::TextureToPixelFormat(TextureFormat textureFormat)
		{
			if (!IsTextureFormatValid(textureFormat))
			{
				LOG_FATAL("Invalid texture format");
				return -1;
			}

			// TODO:  Not tested exhaustively
			switch (textureFormat)
			{
				case TextureFormat::RedGreenBlue8:
				case TextureFormat::RedGreenBlue16:
					return GL_RGB;
				case TextureFormat::RedGreenBlueAlpha8:
				case TextureFormat::RedGreenBlue10A2:
				case TextureFormat::RedGreenBlueAlpha16:
					return GL_RGBA;
				case TextureFormat::Depth16:
				case TextureFormat::Depth24:
					return GL_DEPTH_COMPONENT;
				case TextureFormat::Red8:
				case TextureFormat::Red16:
					return GL_RED;
				case TextureFormat::RedGreen8:
				case TextureFormat::RedGreen16:
					return GL_RG;
				case TextureFormat::Red16f:
				case TextureFormat::Red32f:
					return GL_RED;
				case TextureFormat::RedGreen16f:
				case TextureFormat::RedGreen32f:
					return GL_RG;
				case TextureFormat::Red8i:
				case TextureFormat::Red8ui:
				case TextureFormat::Red16i:
				case TextureFormat::Red16ui:
				case TextureFormat::Red32i:
				case TextureFormat::Red32ui:
					return GL_RED_INTEGER;
				case TextureFormat::RedGreen8i:
				case TextureFormat::RedGreen8ui:
				case TextureFormat::RedGreen16i:
				case TextureFormat::RedGreen16ui:
				case TextureFormat::RedGreen32i:
				case TextureFormat::RedGreen32ui:
					return GL_RG_INTEGER;
				case TextureFormat::RedGreenBlueAlpha32f:
					return GL_RGBA;
				case TextureFormat::RedGreenBlue32f:
					return GL_RGB;
				case TextureFormat::RedGreenBlueAlpha16f:
					return GL_RGBA;
				case TextureFormat::RedGreenBlue16f:
					return GL_RGB;
				case TextureFormat::Depth24Stencil8:
					return GL_DEPTH_STENCIL;
				case TextureFormat::Red11fGreen11fBlue10f:
				case TextureFormat::RedGreenBlue9E5:
					return GL_RGB;
				case TextureFormat::SRedGreenBlue8:
					return GL_RGB_INTEGER;
				case TextureFormat::SRedGreenBlue8Alpha8:
					return GL_RGBA_INTEGER;
				case TextureFormat::Depth32f:
					return GL_DEPTH_COMPONENT;
				case TextureFormat::Depth32fStencil8:
					return GL_DEPTH_STENCIL;
				case TextureFormat::RedGreenBlueAlpha32ui:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue32ui:
					return GL_RGB_INTEGER;
				case TextureFormat::RedGreenBlueAlpha16ui:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue16ui:
					return GL_RGB_INTEGER;
				case TextureFormat::RedGreenBlueAlpha8ui:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue8ui:
					return GL_RGB_INTEGER;
				case TextureFormat::RedGreenBlueAlpha32i:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue32i:
					return GL_RGB_INTEGER;
				case TextureFormat::RedGreenBlueAlpha16i:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue16i:
					return GL_RGB_INTEGER;
				case TextureFormat::RedGreenBlueAlpha8i:
					return GL_RGBA_INTEGER;
				case TextureFormat::RedGreenBlue8i:
					return GL_RGB_INTEGER;
			}

			LOG_FATAL("TextureFormat does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::TextureToPixelType(TextureFormat textureFormat)
		{
			if (!IsTextureFormatValid(textureFormat))
			{
				LOG_FATAL("Invalid texture format::textureFormat");
			}

			// TODO:  Not tested exhaustively
			switch (textureFormat)
			{
				case TextureFormat::RedGreenBlue8:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::RedGreenBlue16:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::RedGreenBlueAlpha8:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::RedGreenBlue10A2:
					return GL_UNSIGNED_INT_10_10_10_2;
				case TextureFormat::RedGreenBlueAlpha16:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::Depth16:
					return GL_HALF_FLOAT;
				case TextureFormat::Depth24:
					return GL_FLOAT;
				case TextureFormat::Red8:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::Red16:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::RedGreen8:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::RedGreen16:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::Red16f:
					return GL_HALF_FLOAT;
				case TextureFormat::Red32f:
					return GL_FLOAT;
				case TextureFormat::RedGreen16f:
					return GL_HALF_FLOAT;
				case TextureFormat::RedGreen32f:
					return GL_FLOAT;
				case TextureFormat::Red8i:
					return GL_BYTE;
				case TextureFormat::Red8ui:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::Red16i:
					return GL_SHORT;
				case TextureFormat::Red16ui:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::Red32i:
					return GL_INT;
				case TextureFormat::Red32ui:
					return GL_UNSIGNED_INT;
				case TextureFormat::RedGreen8i:
					return GL_BYTE;
				case TextureFormat::RedGreen8ui:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::RedGreen16i:
					return GL_SHORT;
				case TextureFormat::RedGreen16ui:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::RedGreen32i:
					return GL_INT;
				case TextureFormat::RedGreen32ui:
					return GL_UNSIGNED_INT;
				case TextureFormat::RedGreenBlueAlpha32f:
					return GL_FLOAT;
				case TextureFormat::RedGreenBlue32f:
					return GL_FLOAT;
				case TextureFormat::RedGreenBlueAlpha16f:
					return GL_HALF_FLOAT;
				case TextureFormat::RedGreenBlue16f:
					return GL_HALF_FLOAT;
				case TextureFormat::Depth24Stencil8:
					return GL_UNSIGNED_INT_24_8;
				case TextureFormat::Red11fGreen11fBlue10f:
					return GL_FLOAT;
				case TextureFormat::RedGreenBlue9E5:
					return GL_FLOAT;
				case TextureFormat::SRedGreenBlue8:
				case TextureFormat::SRedGreenBlue8Alpha8:
					return GL_BYTE;
				case TextureFormat::Depth32f:
				case TextureFormat::Depth32fStencil8:
					return GL_FLOAT;
				case TextureFormat::RedGreenBlueAlpha32ui:
				case TextureFormat::RedGreenBlue32ui:
					return GL_UNSIGNED_INT;
				case TextureFormat::RedGreenBlueAlpha16ui:
				case TextureFormat::RedGreenBlue16ui:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::RedGreenBlueAlpha8ui:
				case TextureFormat::RedGreenBlue8ui:
					return GL_UNSIGNED_BYTE;
				case TextureFormat::RedGreenBlueAlpha32i:
				case TextureFormat::RedGreenBlue32i:
					return GL_UNSIGNED_INT;
				case TextureFormat::RedGreenBlueAlpha16i:
				case TextureFormat::RedGreenBlue16i:
					return GL_UNSIGNED_SHORT;
				case TextureFormat::RedGreenBlueAlpha8i:
				case TextureFormat::RedGreenBlue8i:
					return GL_UNSIGNED_BYTE;
			}

			LOG_FATAL("TextureFormat does not exist.");
			return GL_INVALID_ENUM;
		}

		bool OGLTypeConverter::IsTextureFormatValid(TextureFormat textureFormat)
		{
			return (textureFormat == TextureFormat::RedGreenBlue8) ||
				(textureFormat == TextureFormat::RedGreenBlue16) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha8) ||
				(textureFormat == TextureFormat::RedGreenBlue10A2) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha16) ||
				(textureFormat == TextureFormat::Depth16) ||
				(textureFormat == TextureFormat::Depth24) ||
				(textureFormat == TextureFormat::Red8) ||
				(textureFormat == TextureFormat::Red16) ||
				(textureFormat == TextureFormat::RedGreen8) ||
				(textureFormat == TextureFormat::RedGreen16) ||
				(textureFormat == TextureFormat::Red16f) ||
				(textureFormat == TextureFormat::Red32f) ||
				(textureFormat == TextureFormat::RedGreen16f) ||
				(textureFormat == TextureFormat::RedGreen32f) ||
				(textureFormat == TextureFormat::Red8i) ||
				(textureFormat == TextureFormat::Red8ui) ||
				(textureFormat == TextureFormat::Red16i) ||
				(textureFormat == TextureFormat::Red16ui) ||
				(textureFormat == TextureFormat::Red32i) ||
				(textureFormat == TextureFormat::Red32ui) ||
				(textureFormat == TextureFormat::RedGreen8i) ||
				(textureFormat == TextureFormat::RedGreen8ui) ||
				(textureFormat == TextureFormat::RedGreen16i) ||
				(textureFormat == TextureFormat::RedGreen16ui) ||
				(textureFormat == TextureFormat::RedGreen32i) ||
				(textureFormat == TextureFormat::RedGreen32ui) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha32f) ||
				(textureFormat == TextureFormat::RedGreenBlue32f) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha16f) ||
				(textureFormat == TextureFormat::RedGreenBlue16f) ||
				(textureFormat == TextureFormat::Depth24Stencil8) ||
				(textureFormat == TextureFormat::Red11fGreen11fBlue10f) ||
				(textureFormat == TextureFormat::RedGreenBlue9E5) ||
				(textureFormat == TextureFormat::SRedGreenBlue8) ||
				(textureFormat == TextureFormat::SRedGreenBlue8Alpha8) ||
				(textureFormat == TextureFormat::Depth32f) ||
				(textureFormat == TextureFormat::Depth32fStencil8) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha32ui) ||
				(textureFormat == TextureFormat::RedGreenBlue32ui) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha16ui) ||
				(textureFormat == TextureFormat::RedGreenBlue16ui) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha8ui) ||
				(textureFormat == TextureFormat::RedGreenBlue8ui) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha32i) ||
				(textureFormat == TextureFormat::RedGreenBlue32i) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha16i) ||
				(textureFormat == TextureFormat::RedGreenBlue16i) ||
				(textureFormat == TextureFormat::RedGreenBlueAlpha8i) ||
				(textureFormat == TextureFormat::RedGreenBlue8i);
		}

		GLenum OGLTypeConverter::To(TextureMinificationFilter filter)
		{
			switch (filter)
			{
				case TextureMinificationFilter::Nearest:
					return GL_NEAREST;
				case TextureMinificationFilter::Linear:
					return GL_LINEAR;
				case TextureMinificationFilter::NearestMipmapNearest:
					return GL_NEAREST_MIPMAP_NEAREST;
				case TextureMinificationFilter::LinearMipmapNearest:
					return GL_LINEAR_MIPMAP_NEAREST;
				case TextureMinificationFilter::NearestMipmapLinear:
					return GL_NEAREST_MIPMAP_LINEAR;
				case TextureMinificationFilter::LinearMipmapLinear:
					return GL_LINEAR_MIPMAP_LINEAR;
			}

			LOG_FATAL("TextureMinificationFilter does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::To(TextureMagnificationFilter filter)
		{
			switch (filter)
			{
				case TextureMagnificationFilter::Nearest:
					return GL_NEAREST;
				case TextureMagnificationFilter::Linear:
					return GL_LINEAR;
			}

			LOG_FATAL("TextureMagnificationFilter does not exist.");
			return GL_INVALID_ENUM;
		}

		GLenum OGLTypeConverter::To(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::Clamp:
					return GL_CLAMP_TO_EDGE;
				case TextureWrap::Repeat:
					return GL_REPEAT;
				case TextureWrap::MirroredRepeat:
					return GL_MIRRORED_REPEAT;
			}

			LOG_FATAL("TextureWrap does not exist.");
			return GL_INVALID_ENUM;
		}
	}
}