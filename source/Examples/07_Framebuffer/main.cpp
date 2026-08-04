#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <Resources/BowResources.h>

#include <CoreSystems/BowBasicTimer.h>
#include <CoreSystems/BowLogger.h>

#include <ExampleSupport/ExampleSupport.h>

#include <optick.h>

#include <iostream>

std::string glslVertexShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 varTextureCoord;

void main() {
    gl_Position = vec4(inPosition.xyz, 1.0); // Z-Wert auf 0.0 setzen f�r Vulkan
    varTextureCoord = inTexCoord;
}
)";

std::string glslFragmentShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 varTextureCoord;
layout(binding = 0) uniform sampler2D diffuseTex;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(texture(diffuseTex, varTextureCoord).rgb, 1.0);
}
)";

void SaveImageToDisk(void *data, uint32_t width, uint32_t height, const std::string &filename)
{
    // Save to file (e.g., as a .ppm file)
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file << "P6\n" << width << " " << height << "\n255\n";
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            file.write((char *)data + (y * width + x) * 4, 3); // only write RGB, not alpha
        }
    }

    file.close();
}

int main(int argc, char *argv[])
{
    FN("main");

    OPTICK_APP("Framebuffer Sample");

    ///////////////////////////////////////////////////////////////////
    // Creating Render Device and Window

    bow::RenderDeviceAPI backend = bow::examples::SelectBackend(argc, argv);
    LOG_INFO("Render backend: %s", bow::examples::BackendName(backend));

    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(backend);
    if (device == nullptr)
    {
        LOG_ERROR("Could not create device!");
        return -1;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(512, 512, "Triangle", bow::WindowType::Windowed);
    if (window == nullptr)
    {
        LOG_ERROR("Could not create window!");
        return -1;
    }
    bow::RenderContextPtr context = window->VGetContext();

    ///////////////////////////////////////////////////////////////////
    // Input

    bow::KeyboardPtr keyboard = bow::InputDeviceManager::GetInstance().CreateKeyboardObject(window);
    if (keyboard == nullptr)
    {
        LOG_ERROR("Could not create keyboard object!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Shader

    bow::ShaderProgramPtr shaderProgram = device->VCreateShaderProgram(glslVertexShader, glslFragmentShader);
    if (shaderProgram == nullptr)
    {
        LOG_ERROR("Could not create shader program!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // ClearState and Color

    bow::ClearState clearBlue;
    clearBlue.color = bow::ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

    bow::ClearState clearBlack;
    clearBlack.color = bow::ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f);

    bow::ClearState clearRed;
    clearBlack.color = bow::ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);

    ///////////////////////////////////////////////////////////////////
    // RenderState

    bow::RenderState renderState;
    renderState.faceCulling.Enabled = false;
    renderState.depthTest.Enabled = false;

    ///////////////////////////////////////////////////////////////////
    // Vertex Array

    bow::Vector3<float> vertices[6];
    vertices[0] = bow::Vector3<float>(-1.0f, -1.0f, 0.0f);
    vertices[1] = bow::Vector3<float>(1.0f, -1.0f, 0.0f);
    vertices[2] = bow::Vector3<float>(1.0f, 1.0f, 0.0f);
    vertices[3] = bow::Vector3<float>(1.0f, 1.0f, 0.0f);
    vertices[4] = bow::Vector3<float>(-1.0f, 1.0f, 0.0f);
    vertices[5] = bow::Vector3<float>(-1.0f, -1.0f, 0.0f);

    // fill buffer with informations
    bow::VertexBufferAttributePtr positionAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector3<float>) * 6), bow::ComponentDatatype::Float, 3));
    positionAttribute->GetVertexBuffer()->VCopyFromSystemMemory(vertices, 0, sizeof(bow::Vector3<float>) * 6);

    bow::Vector2<float> texcoor[6];
    texcoor[0] = bow::Vector2<float>(0.0f, 0.0f);
    texcoor[1] = bow::Vector2<float>(1.0f, 0.0f);
    texcoor[2] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[3] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[4] = bow::Vector2<float>(0.0f, 1.0f);
    texcoor[5] = bow::Vector2<float>(0.0f, 0.0f);

    bow::VertexBufferAttributePtr textureCoordAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector2<float>) * 6), bow::ComponentDatatype::Float, 2));
    textureCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(texcoor, 0, sizeof(bow::Vector2<float>) * 6);

    // create VertexAttributeBindings and connect buffer with location
    bow::VertexAttributeBindingsPtr vertexAttributeBindings = context->VCreateVertexAttributeBindings();
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inPosition"), positionAttribute);
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inTexCoord"), textureCoordAttribute);

    ///////////////////////////////////////////////////////////////////
    // Vertex Array

    bow::Vector3<float> verticesSmall[6];
    verticesSmall[0] = bow::Vector3<float>(-0.5f, -0.5f, 1.0f);
    verticesSmall[1] = bow::Vector3<float>(0.5f, -0.5f, 1.0f);
    verticesSmall[2] = bow::Vector3<float>(0.5f, 0.5f, 1.0f);
    verticesSmall[3] = bow::Vector3<float>(0.5f, 0.5f, 1.0f);
    verticesSmall[4] = bow::Vector3<float>(-0.5f, 0.5f, 1.0f);
    verticesSmall[5] = bow::Vector3<float>(-0.5f, -0.5f, 1.0f);

    // fill buffer with informations
    bow::VertexBufferAttributePtr positionSmallAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector3<float>) * 6), bow::ComponentDatatype::Float, 3));
    positionSmallAttribute->GetVertexBuffer()->VCopyFromSystemMemory(verticesSmall, 0, sizeof(bow::Vector3<float>) * 6);

    bow::Vector2<float> texcoorSmall[6];
    texcoorSmall[0] = bow::Vector2<float>(0.0f, 0.0f);
    texcoorSmall[1] = bow::Vector2<float>(1.0f, 0.0f);
    texcoorSmall[2] = bow::Vector2<float>(1.0f, 1.0f);
    texcoorSmall[3] = bow::Vector2<float>(1.0f, 1.0f);
    texcoorSmall[4] = bow::Vector2<float>(0.0f, 1.0f);
    texcoorSmall[5] = bow::Vector2<float>(0.0f, 0.0f);

    bow::VertexBufferAttributePtr textureSmallCoordAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector2<float>) * 6), bow::ComponentDatatype::Float, 2));
    textureSmallCoordAttribute->GetVertexBuffer()->VCopyFromSystemMemory(texcoorSmall, 0, sizeof(bow::Vector2<float>) * 6);

    // create VertexAttributeBindings and connect buffer with location
    bow::VertexAttributeBindingsPtr vertexAttributeBindingsSmall = context->VCreateVertexAttributeBindings();
    vertexAttributeBindingsSmall->VSetAttribute(shaderProgram->VGetVertexAttribute("inPosition"), positionSmallAttribute);
    vertexAttributeBindingsSmall->VSetAttribute(shaderProgram->VGetVertexAttribute("inTexCoord"), textureSmallCoordAttribute);

    ///////////////////////////////////////////////////////////////////
    // Textures

    bow::Texture2DPtr texture = device->VCreateTexture2D(bow::ImageManager::GetInstance().Load(bow::examples::DataPath("Textures/test.bmp")));
    bow::TextureSamplerPtr sampler = device->VCreateTexture2DSampler(bow::TextureMinificationFilter::Linear, bow::TextureMagnificationFilter::Linear, bow::TextureWrap::Clamp, bow::TextureWrap::Clamp);

    ///////////////////////////////////////////////////////////////////
    // FrameBuffer

    int out_Color_Location = shaderProgram->VGetFragmentOutputLocation("outColor");
    bow::Texture2DPtr renderTarget = device->VCreateTexture2D(bow::Texture2DDescription(window->VGetWidth(), window->VGetHeight(), bow::TextureFormat::RedGreenBlueAlpha8));
    // bow::Texture2DPtr depthTarget = device->VCreateTexture2D(bow::Texture2DDescription(window->VGetWidth(), window->VGetHeight(), bow::TextureFormat::Depth32f));

    bow::FramebufferPtr frameBuffer = context->VCreateFramebuffer();
    frameBuffer->VSetColorAttachment(out_Color_Location, renderTarget);
    // frameBuffer->VSetDepthAttachment(depthTarget);

    ///////////////////////////////////////////////////////////////////
    // Render to Framebuffer

    // Textures are no longer bound on the context by slot index; they are set
    // on a resource binding object obtained from the shader program. The two
    // passes sample different textures through the same sampler binding, so
    // each one gets its own object rather than being rebound every frame.

    bow::ShaderResourceBindingsPtr offscreenBindings = shaderProgram->VCreateResourceBindingObjects();
    offscreenBindings->VSetTexture("diffuseTex", texture, sampler);

    bow::ShaderResourceBindingsPtr presentBindings = shaderProgram->VCreateResourceBindingObjects();
    presentBindings->VSetTexture("diffuseTex", renderTarget, sampler);

    const uint32_t quadVertexCount = 6;

    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        // Render the source texture into the offscreen target
        context->VSetFramebuffer(frameBuffer);
        context->VClear(clearBlue);
        context->VSetViewport(bow::Viewport(0, 0, window->VGetWidth(), window->VGetHeight()));
        context->VDraw(bow::PrimitiveType::Triangles, 0, quadVertexCount, vertexAttributeBindings, offscreenBindings, shaderProgram, renderState);

        // Render that offscreen target to the screen
        context->VSetFramebuffer(nullptr);
        context->VClear(clearRed);
        context->VSetViewport(bow::Viewport(0, 0, window->VGetWidth(), window->VGetHeight()));
        context->VDraw(bow::PrimitiveType::Triangles, 0, quadVertexCount, vertexAttributeBindings, presentBindings, shaderProgram, renderState);

        context->VSwapBuffers();
    }

    ///////////////////////////////////////////////////////////////////
    // Copy Framebuffer Content to Texture

    LOG_INFO("renderTarget->VCopyToSystemMemory");
    std::shared_ptr<void> framebufferContent = renderTarget->VCopyToSystemMemory(bow::ImageFormat::BlueGreenRedAlpha, bow::ImageDatatype::UnsignedByte);

    SaveImageToDisk(framebufferContent.get(), window->VGetWidth(), window->VGetHeight(), "output.ppm");

    OPTICK_SHUTDOWN();

    return 0;
}
