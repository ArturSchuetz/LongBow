#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/BowGraphicsWindow.h>
#include <RenderDevice/Device/IBowRenderContext.h>

#include <CoreSystems/BowCorePredeclares.h>
#include <CoreSystems/BowMath.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace bow
{

class VulkanRenderSurface : public IRenderContext
{
  public:
    VulkanRenderSurface(GLFWwindow *window);
    ~VulkanRenderSurface();

    // =========================================================================
    // Init/Release stuff

    bool InitializeSurface(VulkanRenderDevice *renderDevice);
    bool InitializeSwapchain(uint16_t width, uint16_t height);
    void VRelease();

    // =========================================================================
    // Inherited via IRenderContext

    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshAttribute mesh, ShaderVertexAttributeMap shaderAttributes, BufferHint usageHint) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings(MeshBufferPtr meshBuffers) override;
    VertexAttributeBindingsPtr VCreateVertexAttributeBindings() override;
    FramebufferPtr VCreateFramebuffer() override;

    void VBeginFrame() override;
    void VEndFrame() override;

    void VClear(ClearState clearState) override;
    void VDraw(PrimitiveType primitiveType, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDraw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderResourceBindingsPtr shaderResourceBindings, ShaderProgramPtr shaderProgram, RenderState renderState) override;
    void VDrawLine(const bow::Vector3<float> &start, const bow::Vector3<float> &end) override;

    void VSetFramebuffer(FramebufferPtr framebufer) override;
    void VSetViewport(Viewport viewport) override;
    Viewport VGetViewport() override;

    void VSwapBuffers(bool vsync) override;

    // ==================== RayTracing ====================

    void VTraceRays(void *shaderProgram, ShaderResourceBindingsPtr resourceBindings, Texture2DPtr outputImage, uint32_t width, uint32_t height) override;

    // =========================================================================
    // Vulkan specific

    VkSurfaceKHR GetHandle() const;

  private:
    // you shall not copy!
    VulkanRenderSurface(const VulkanRenderSurface &) = delete;
    VulkanRenderSurface &operator=(const VulkanRenderSurface &) = delete;

    void Draw(PrimitiveType primitiveType, uint32_t offset, uint32_t count, VertexAttributeBindingsPtr vertexAttributeBindings, ShaderProgramPtr shaderProgram, RenderState renderState, const std::vector<VkDescriptorSet> &descriptorSets);

    void PrepareCommandBuffer();
    void FinishCommandBuffer();

    void PrepareRenderPass(bool clearPass = false);
    bool FinishRenderPass();

    bool RecreateSwapchain(uint16_t width, uint16_t height);

    void ApplyVertexAttributeBindings(VertexAttributeBindingsPtr vertexAttributeBindings);

    VulkanRenderDevice *m_renderDevice;
    VulkanSwapchainPtr m_swapchain;
    Viewport m_viewport;
    bool m_vsync;
    VulkanFramebufferPtr m_boundFramebuffer;
    VkSurfaceKHR m_surface;
    GLFWwindow *m_window;
    static VulkanRenderSurface *m_currentSurface;
    VulkanRenderPassPtr m_renderPass;

    ColorRGBA m_clearColor;
    float m_clearDepth;
    float m_clearStencil;

    bool m_clearColorBuffer;
    bool m_clearDepthBuffer;
    bool m_clearStencilBuffer;

    VertexAttributeBindingsPtr m_currentlyBindedVertexAttributeBindings;

    std::string m_guid;
};

} // namespace bow
