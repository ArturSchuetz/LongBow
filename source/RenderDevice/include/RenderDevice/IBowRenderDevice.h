#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>
#include <RenderDevice/Device/RayTracing/IBowBottomLevelAccelerationStructure.h>
#include <RenderDevice/Device/RayTracing/IBowRayTracingShaderProgram.h>
#include <RenderDevice/Device/RayTracing/IBowTopLevelAccelerationStructure.h>

#include <CoreSystems/BowCorePredeclares.h>
#include <Resources/BowResourcesPredeclares.h>

#include <memory>
#include <string>

namespace bow
{

enum class WindowType : char
{
    Windowed = 0,
    FullscreenBorderlessWindow,
    Fullscreen
};

class IRenderDevice
{
  public:
    IRenderDevice() {};
    virtual ~IRenderDevice() {}
    virtual void VRelease() = 0;

    virtual GraphicsWindowPtr VCreateWindow(int width = 800, int height = 600, const std::string &title = "LongBow Game", WindowType type = WindowType::Windowed) = 0;

    virtual ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragementShaderFilename) = 0;

    virtual ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &GeometryShaderFilename, const std::string &FragementShaderFilename) = 0;

    virtual ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &TessControlShaderFilename, const std::string &TessEvalShaderFilename) = 0;

    virtual ShaderProgramPtr VCreateShaderProgramFromFile(const std::string &VertexShaderFilename, const std::string &FragmentShaderFilename, const std::string &GeometryShaderFilename, const std::string &TessControlShaderFilename,
                                                          const std::string &TessEvalShaderFilename) = 0;

    virtual ComputeShaderProgramPtr VCreateComputeShaderProgram(const std::string &computeShaderSource) = 0;

    virtual ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragementShaderSource) = 0;

    virtual ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &GeometryShaderSource, const std::string &FragementShaderSource) = 0;

    virtual ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &TessControlShaderSource, const std::string &TessEvalShaderSource) = 0;

    virtual ShaderProgramPtr VCreateShaderProgram(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &GeometryShaderSource, const std::string &TessControlShaderSource,
                                                  const std::string &TessEvalShaderSource) = 0;

    virtual MeshBufferPtr VCreateMeshBuffers(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) = 0;
    virtual VertexBufferPtr VCreateVertexBuffer(BufferHint usageHint, int sizeInBytes, bool useForRayTracing = false) = 0;
    virtual IndexBufferPtr VCreateIndexBuffer(BufferHint usageHint, IndexBufferDatatype dataType, int sizeInBytes, bool useForRayTracing = false) = 0;
    virtual WritePixelBufferPtr VCreateWritePixelBuffer(PixelBufferHint usageHint, int sizeInBytes) = 0;
    virtual UniformBufferPtr VCreateUniformBuffer(BufferHint usageHint, int sizeInBytes, void *data = nullptr) = 0;
    virtual StorageBufferPtr VCreateStorageBuffer(BufferHint usageHint, int sizeInBytes, void *data = nullptr) = 0;

    virtual Texture2DPtr VCreateTexture2D(Texture2DDescription description) = 0;
    virtual Texture2DPtr VCreateTexture2D(ImagePtr image) = 0;

    virtual TextureSamplerPtr VCreateTexture2DSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1) = 0;

    // ==================== RayTracing ====================

    virtual std::unique_ptr<IRayTracingShaderProgram> VCreateRayTracingShaderProgram(const std::string &rayGenShaderSource, const std::string &anyHitShaderSource, const std::string &closestHitShaderSource, const std::string &missShaderSource,
                                                                                     const std::string &intersectionShaderSource, const std::string &callableShaderSource) = 0;

    virtual std::unique_ptr<IBottomLevelAccelerationStructure> VCreateBottomLevelAccelerationStructure(MeshPtr mesh) = 0;

    virtual std::unique_ptr<ITopLevelAccelerationStructure> VCreateTopLevelAccelerationStructure(const std::vector<std::unique_ptr<IBottomLevelAccelerationStructure>> &bottomLevelAccelerationStructures) = 0;
};

} // namespace bow
