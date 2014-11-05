#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace Bow {
	namespace Renderer{

		class Camera;
		class RenderDeviceManager;
			enum class API : char;

		////////////////////////////////////////////////////
		// Buffer

		enum class BufferHint : char;
		enum class PixelBufferHint : char;
		enum class IndexBufferDatatype : char;

		struct VertexBufferAttribute;
			enum class ComponentDatatype : char;

		typedef std::shared_ptr<VertexBufferAttribute> VertexBufferAttributePtr;
		typedef std::unordered_map<int, VertexBufferAttributePtr> VertexBufferAttributeMap;

		////////////////////////////////////////////////////
		// Shader

		struct ShaderVertexAttribute;
			enum class ShaderVertexAttributeType : char;
		typedef std::shared_ptr<ShaderVertexAttribute> ShaderVertexAttributePtr;
		typedef std::unordered_map<std::string, ShaderVertexAttributePtr> ShaderVertexAttributeMap;


		////////////////////////////////////////////////////
		// Clearstate

		struct ClearState;
			typedef enum TYPE_ClearBuffers  ClearBuffers;

		////////////////////////////////////////////////////
		// RenderState
		
		struct RenderState;
			enum class ProgramPointSize : char;
			enum class RasterizationMode : char;
			struct Blending;
				enum class SourceBlendingFactor : char;
				enum class DestinationBlendingFactor : char;
				enum class BlendEquation : char;
			struct ColorMask;
			struct DepthRange;
			struct DepthTest;
				enum class DepthTestFunction : char;
			struct FaceCulling;
				enum class CullFace : char;
				enum class WindingOrder : char;
			struct PrimitiveRestart;
			struct ScissorTest;
			struct StencilTest;
			struct StencilTestFace;
				enum class StencilOperation : char;
				enum class StencilTestFunction : char;

		////////////////////////////////////////////////////
		// INTERFACES

				////////////////////////////////////////////////////
				// Buffer
				class IIndexBuffer;
				typedef std::shared_ptr<IIndexBuffer> IndexBufferPtr;

				class IReadPixelBuffer;
				typedef std::shared_ptr<IReadPixelBuffer> ReadPixelBufferPtr;

				class IVertexBuffer;
				typedef std::shared_ptr<IVertexBuffer> VertexBufferPtr;

				class IWritePixelBuffer;
				typedef std::shared_ptr<IWritePixelBuffer> WritePixelBufferPtr;

				////////////////////////////////////////////////////
				// FrameBuffer
				class IColorAttachments;
				typedef std::shared_ptr<IColorAttachments> ColorAttachmentsPtr;

				class IFramebuffer;
				typedef std::shared_ptr<IFramebuffer> FramebufferPtr;

				////////////////////////////////////////////////////
				// VertexArray
				class IVertexArray;
				typedef std::shared_ptr<IVertexArray> VertexArrayPtr;

				////////////////////////////////////////////////////
				// Shader
				class IFragmentOutputs;
				typedef std::shared_ptr<IFragmentOutputs> FragmentOutputsPtr;

				class IShaderProgram;
				typedef std::shared_ptr<IShaderProgram> ShaderProgramPtr;

				////////////////////////////////////////////////////
				// Textures

				struct Texture2DDescription;
					enum class TextureFormat : char;

				class ITexture2D;
					enum class ImageDatatype : char;
					enum class ImageFormat : char;
				typedef std::shared_ptr<ITexture2D> Texture2DPtr;

				class ITextureSampler;
					enum class TextureMinificationFilter : char;
					enum class TextureMagnificationFilter : char;
					enum class TextureWrap : char;
				typedef std::shared_ptr<ITextureSampler> TextureSamplerPtr;

			class IGraphicsWindow;
			typedef std::shared_ptr<IGraphicsWindow> GraphicsWindowPtr;

			class IRenderContext;
				enum class PrimitiveType : char;
				struct Viewport;
			typedef std::shared_ptr<IRenderContext> RenderContextPtr;

			class IRenderDevice;
				enum class WindowType : char;
			typedef std::shared_ptr<IRenderDevice> RenderDevicePtr;
	}
}
