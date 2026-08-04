#pragma once
#include <RenderDevice/RenderDevice_api.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace bow
{
class Camera;
class RENDERDEVICE_API RenderDeviceManager;
enum class RenderDeviceAPI : char;

////////////////////////////////////////////////////
// Buffer

enum class BufferHint : char;
enum class PixelBufferHint : char;
enum class IndexBufferDatatype : char;
enum class ComponentDatatype : char;

struct VertexBufferAttribute;
typedef std::shared_ptr<VertexBufferAttribute> VertexBufferAttributePtr;
typedef std::unordered_map<uint32_t, VertexBufferAttributePtr> VertexBufferAttributeMap;

////////////////////////////////////////////////////
// Shader

enum class ShaderStage : uint8_t;

struct ShaderVertexAttribute;
enum class ShaderVertexAttributeType : char;
typedef std::shared_ptr<ShaderVertexAttribute> ShaderVertexAttributePtr;
typedef std::unordered_map<uint32_t, ShaderVertexAttributePtr> ShaderVertexAttributeMap;

class IShaderResourceBindings;
typedef std::shared_ptr<IShaderResourceBindings> ShaderResourceBindingsPtr;

class IFragmentOutputs;
typedef std::shared_ptr<IFragmentOutputs> FragmentOutputsPtr;

class IComputeShaderProgram;
typedef std::shared_ptr<IComputeShaderProgram> ComputeShaderProgramPtr;

class IShaderProgram;
typedef std::shared_ptr<IShaderProgram> ShaderProgramPtr;

////////////////////////////////////////////////////
// Clearstate

struct ClearState;
enum TYPE_ClearBuffers : char;
typedef TYPE_ClearBuffers ClearBuffers;

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

class IUniformBuffer;
typedef std::shared_ptr<IUniformBuffer> UniformBufferPtr;

class IStorageBuffer;
typedef std::shared_ptr<IStorageBuffer> StorageBufferPtr;

////////////////////////////////////////////////////
// FrameBuffer
class IColorAttachments;
typedef std::shared_ptr<IColorAttachments> ColorAttachmentsPtr;

class IFramebuffer;
typedef std::shared_ptr<IFramebuffer> FramebufferPtr;

////////////////////////////////////////////////////
// Mesh
class MeshBuffers;
typedef std::shared_ptr<MeshBuffers> MeshBufferPtr;

////////////////////////////////////////////////////
// VertexAttributeBindings
class IVertexAttributeBindings;
typedef std::shared_ptr<IVertexAttributeBindings> VertexAttributeBindingsPtr;

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
enum class PrimitiveType : uint8_t;
struct Viewport;
typedef std::shared_ptr<IRenderContext> RenderContextPtr;

class IRenderDevice;
enum class WindowType : char;
typedef std::shared_ptr<IRenderDevice> RenderDevicePtr;

} // namespace bow
