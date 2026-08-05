// A triangle on the thin interface.
//
// The point of this example is the shape of the frame: nothing is drawn when a
// call is made. Commands are recorded into a list, the list is submitted to a
// queue, and a timeline fence says when the GPU is done with it. That is the
// whole difference from the classic tier, where the driver decides when work
// is sent.

#include <ThinRenderDevice/BowThinRenderDeviceManager.h>
#include <ThinRenderDevice/IBowThinDevice.h>

#include <CoreSystems/BowLogger.h>

#include <cstdlib>
#include <fstream>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace
{

//! Reads a compiled shader produced by the build.
std::vector<char> ReadShader(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        LOG_ERROR("Could not open %s. Was the build run so the shaders were compiled?", path.c_str());
        return {};
    }

    const std::streamsize size = file.tellg();
    file.seekg(0);

    std::vector<char> bytes((size_t)size);
    file.read(bytes.data(), size);
    return bytes;
}

struct Vertex
{
    float position[3];
    float color[3];
};

//! How many frames may be recorded before waiting for the first to finish.
/*!
    With one frame in flight the CPU waits for the GPU every frame. Two lets
    them overlap, which is the reason the thin tier exposes fences at all.
*/
const uint32_t FramesInFlight = 2;

} // namespace

int main(int argc, char *argv[])
{
    FN("main");

    bow::ThinRenderDeviceAPI api = bow::ThinRenderDeviceAPI::Vulkan;

    // Render a fixed number of frames and exit, so the example can be run
    // unattended -- by a build server, or to check it draws at all.
    uint64_t frameLimit = 0;

    for (int i = 1; i < argc; ++i)
    {
        const std::string argument = argv[i];
        if (argument == "--backend" && i + 1 < argc)
        {
            const std::string name = argv[i + 1];
            if (name == "directx12" || name == "dx12")
            {
                api = bow::ThinRenderDeviceAPI::DirectX12;
            }
        }
        else if (argument == "--frames" && i + 1 < argc)
        {
            frameLimit = (uint64_t)std::strtoull(argv[i + 1], nullptr, 10);
        }
    }

    bow::ThinDevicePtr device = bow::ThinRenderDeviceManager::GetInstance().CreateDevice(api);
    if (device == nullptr)
    {
        LOG_ERROR("Could not create a thin device.");
        return -1;
    }

    LOG_INFO("Thin device on %s", device->VGetCapabilities().adapterName.c_str());

    if (!glfwInit())
    {
        LOG_ERROR("Could not initialise GLFW.");
        return -1;
    }

    // The thin tier does not create windows: it takes a native handle, so the
    // application is free to use whatever windowing it already has.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Hello Triangle (thin)", nullptr, nullptr);
    if (window == nullptr)
    {
        LOG_ERROR("Could not create a window.");
        return -1;
    }

    bow::ThinSwapchainPtr swapchain = device->VCreateSwapchain(glfwGetWin32Window(window), 800, 600, FramesInFlight);
    if (swapchain == nullptr)
    {
        LOG_ERROR("Could not create a swapchain.");
        return -1;
    }

    bow::ThinQueuePtr queue = device->VGetQueue(bow::ThinQueueType::Graphics);
    bow::ThinFencePtr frameFence = device->VCreateFence(0);

    // One pool per frame in flight. Resetting a pool recycles every list it
    // handed out, so a single shared pool could only be reset once the GPU had
    // finished everything -- which would defeat the point of overlapping
    // frames at all.
    std::vector<bow::ThinCommandPoolPtr> commandPools;
    for (uint32_t i = 0; i < FramesInFlight; ++i)
    {
        commandPools.push_back(device->VCreateCommandPool(bow::ThinQueueType::Graphics));
    }

    // Both forms came from the same HLSL file; only the extension differs.
    const char *suffix = (api == bow::ThinRenderDeviceAPI::DirectX12) ? ".dxil" : ".spv";
    const std::vector<char> vertexByteCode = ReadShader(std::string("shaders/Triangle.vertex") + suffix);
    const std::vector<char> fragmentByteCode = ReadShader(std::string("shaders/Triangle.fragment") + suffix);
    if (vertexByteCode.empty() || fragmentByteCode.empty())
    {
        return -1;
    }

    bow::ThinShaderModulePtr vertexShader = device->VCreateShaderModule(vertexByteCode.data(), vertexByteCode.size(), bow::ThinShaderStage::Vertex, "mainVS");
    bow::ThinShaderModulePtr fragmentShader = device->VCreateShaderModule(fragmentByteCode.data(), fragmentByteCode.size(), bow::ThinShaderStage::Fragment, "mainPS");
    if (vertexShader == nullptr || fragmentShader == nullptr)
    {
        return -1;
    }

    const Vertex vertices[] = {
        {{0.0f, 0.6f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.6f, -0.6f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.6f, -0.6f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    };

    bow::ThinBufferDescription vertexBufferDescription;
    vertexBufferDescription.sizeInBytes = sizeof(vertices);
    vertexBufferDescription.vertexBuffer = true;
    // Written once from the CPU and small, so host-visible memory avoids a
    // staging copy for no meaningful cost.
    vertexBufferDescription.cpuVisible = true;
    vertexBufferDescription.debugName = "triangle vertices";

    bow::ThinBufferPtr vertexBuffer = device->VCreateBuffer(vertexBufferDescription);
    if (vertexBuffer == nullptr)
    {
        return -1;
    }

    if (void *mapped = vertexBuffer->VMap())
    {
        memcpy(mapped, vertices, sizeof(vertices));
        vertexBuffer->VUnmap();
    }

    // No descriptor sets and no root constants: the triangle carries its own
    // colours, so the layout is empty.
    bow::ThinPipelineLayoutPtr layout = device->VCreatePipelineLayout(bow::ThinPipelineLayoutDescription());

    bow::ThinGraphicsPipelineDescription pipelineDescription;
    pipelineDescription.vertexShader = vertexShader;
    pipelineDescription.fragmentShader = fragmentShader;
    pipelineDescription.layout = layout;
    pipelineDescription.cullMode = bow::ThinCullMode::None;
    pipelineDescription.colorAttachmentFormats.push_back(swapchain->VGetFormat());
    pipelineDescription.colorAttachmentBlends.resize(1);

    bow::ThinVertexBufferBinding binding;
    binding.binding = 0;
    binding.strideInBytes = sizeof(Vertex);
    pipelineDescription.vertexBuffers.push_back(binding);

    bow::ThinVertexAttribute position;
    position.location = 0;
    position.format = bow::ThinFormat::RGB32Float;
    position.offsetInBytes = offsetof(Vertex, position);
    pipelineDescription.vertexAttributes.push_back(position);

    bow::ThinVertexAttribute color;
    color.location = 1;
    color.format = bow::ThinFormat::RGB32Float;
    color.offsetInBytes = offsetof(Vertex, color);
    color.semanticIndex = 1;
    pipelineDescription.vertexAttributes.push_back(color);

    bow::ThinPipelinePtr pipeline = device->VCreateGraphicsPipeline(pipelineDescription);
    if (pipeline == nullptr)
    {
        return -1;
    }

    uint64_t frame = 0;

    while (!glfwWindowShouldClose(window))
    {
        if (frameLimit > 0 && frame >= frameLimit)
        {
            break;
        }

        glfwPollEvents();

        const uint32_t slot = (uint32_t)(frame % FramesInFlight);

        // Wait until the frame that last used this slot has finished, so its
        // pool can be reset. This is the whole synchronisation the example
        // needs, and it is why the interface exposes a timeline fence.
        if (frame >= FramesInFlight)
        {
            frameFence->VWait(frame - FramesInFlight + 1);
        }

        const uint32_t imageIndex = swapchain->VAcquireNextImage();
        bow::ThinTexturePtr backBuffer = swapchain->VGetImage(imageIndex);

        commandPools[slot]->VReset();
        bow::ThinCommandListPtr commandList = commandPools[slot]->VAllocate();

        commandList->VBegin();
        commandList->VBeginDebugLabel("triangle");

        // The image comes back from the swapchain in an undefined state and
        // has to be announced as a render target, then as presentable. Nothing
        // infers these.
        bow::ThinBarrier toRenderTarget;
        toRenderTarget.texture = backBuffer;
        toRenderTarget.before = bow::ThinResourceState::Undefined;
        toRenderTarget.after = bow::ThinResourceState::RenderTarget;
        commandList->VBarrier({toRenderTarget});

        bow::ThinRenderingInfo rendering;
        rendering.width = swapchain->VGetWidth();
        rendering.height = swapchain->VGetHeight();

        bow::ThinColorAttachment attachment;
        attachment.texture = backBuffer;
        attachment.loadOp = bow::ThinLoadOp::Clear;
        attachment.clearColor[0] = 0.392f;
        attachment.clearColor[1] = 0.584f;
        attachment.clearColor[2] = 0.929f;
        rendering.colorAttachments.push_back(attachment);

        commandList->VBeginRendering(rendering);

        bow::ThinViewport viewport;
        viewport.width = (float)rendering.width;
        viewport.height = (float)rendering.height;
        commandList->VSetViewport(viewport);

        bow::ThinScissor scissor;
        scissor.width = rendering.width;
        scissor.height = rendering.height;
        commandList->VSetScissor(scissor);

        commandList->VBindPipeline(pipeline);
        commandList->VBindVertexBuffer(0, vertexBuffer);
        commandList->VDraw(3, 1, 0, 0);

        commandList->VEndRendering();

        bow::ThinBarrier toPresent;
        toPresent.texture = backBuffer;
        toPresent.before = bow::ThinResourceState::RenderTarget;
        toPresent.after = bow::ThinResourceState::Present;
        commandList->VBarrier({toPresent});

        commandList->VEndDebugLabel();
        commandList->VEnd();

        ++frame;
        queue->VSubmit({commandList}, nullptr, 0, frameFence, frame);

        swapchain->VPresent();
    }

    device->VWaitIdle();

    LOG_INFO("Rendered %llu frames.", (unsigned long long)frame);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
