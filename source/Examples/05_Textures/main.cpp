#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <Resources/BowResources.h>

#include <CoreSystems/BowBasicTimer.h>
#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <iostream>

std::string glslVertexShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 varTextureCoord;

void main() {
    gl_Position = vec4(inPosition.xyz, 1.0);
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

int main(int /*argc*/, char * /*argv[]*/)
{
    FN("main");

    OPTICK_APP("Textures Sample");

    // Creating Render Device
    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(bow::RenderDeviceAPI::Vulkan);
    if (device == nullptr)
    {
        return 0;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 800, "Textures Sample", bow::WindowType::Windowed);
    if (window == nullptr)
    {
        return 0;
    }

    bow::RenderContextPtr context = window->VGetContext();

    bow::ShaderProgramPtr shaderProgram = device->VCreateShaderProgram(glslVertexShader, glslFragmentShader);
    if (shaderProgram == nullptr)
    {
        LOG_ERROR("Could not create shader program!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // ClearState and Color

    bow::ClearState clearState;
    clearState.color = bow::ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

    ///////////////////////////////////////////////////////////////////
    // Vertex Attribute Bindings

    bow::Vector3<float> vertices[12];
    vertices[0] = bow::Vector3<float>(-1.0f, -1.0f, 0.0f);
    vertices[1] = bow::Vector3<float>(1.0f, -1.0f, 0.0f);
    vertices[2] = bow::Vector3<float>(1.0f, 1.0f, 0.0f);
    vertices[3] = bow::Vector3<float>(1.0f, 1.0f, 0.0f);
    vertices[4] = bow::Vector3<float>(-1.0f, 1.0f, 0.0f);
    vertices[5] = bow::Vector3<float>(-1.0f, -1.0f, 0.0f);

    vertices[6] = bow::Vector3<float>(-0.5f, -0.5f, 1.0f);
    vertices[7] = bow::Vector3<float>(0.5f, -0.5f, 1.0f);
    vertices[8] = bow::Vector3<float>(0.5f, 0.5f, 1.0f);
    vertices[9] = bow::Vector3<float>(0.5f, 0.5f, 1.0f);
    vertices[10] = bow::Vector3<float>(-0.5f, 0.5f, 1.0f);
    vertices[11] = bow::Vector3<float>(-0.5f, -0.5f, 1.0f);

    // Create vertex position buffer and fill with informations
    bow::VertexBufferPtr positionBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector3<float>) * 12);
    positionBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(bow::Vector3<float>) * 12);

    // Define buffer as vertexattribute for shaders
    bow::VertexBufferAttributePtr positionAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(positionBuffer, bow::ComponentDatatype::Float, 3));

    bow::Vector2<float> texcoor[12];
    texcoor[0] = bow::Vector2<float>(0.0f, 0.0f);
    texcoor[1] = bow::Vector2<float>(1.0f, 0.0f);
    texcoor[2] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[3] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[4] = bow::Vector2<float>(0.0f, 1.0f);
    texcoor[5] = bow::Vector2<float>(0.0f, 0.0f);

    texcoor[6] = bow::Vector2<float>(0.0f, 0.0f);
    texcoor[7] = bow::Vector2<float>(1.0f, 0.0f);
    texcoor[8] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[9] = bow::Vector2<float>(1.0f, 1.0f);
    texcoor[10] = bow::Vector2<float>(0.0f, 1.0f);
    texcoor[11] = bow::Vector2<float>(0.0f, 0.0f);

    // Create vertex texturecoodinate buffer and fill with informations
    bow::VertexBufferPtr textureCoordBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector2<float>) * 12);
    textureCoordBuffer->VCopyFromSystemMemory(texcoor, 0, sizeof(bow::Vector2<float>) * 12);

    // Define buffer as vertexattribute for shaders
    bow::VertexBufferAttributePtr textureCoordAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(textureCoordBuffer, bow::ComponentDatatype::Float, 2));

    // create VertexAttributeBindings and connect attributeBuffers with location
    bow::VertexAttributeBindingsPtr vertexAttributeBindings = context->VCreateVertexAttributeBindings();
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inPosition")->Location, positionAttribute);
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inTexCoord")->Location, textureCoordAttribute);

    ///////////////////////////////////////////////////////////////////
    // Textures

    bow::Texture2DPtr texture = device->VCreateTexture2D(bow::ImageManager::GetInstance().Load("F:/Projects/masterthesis/data/Scenes/Sponza/textures/vase_plant_mask.png"));
    bow::TextureSamplerPtr sampler = device->VCreateTexture2DSampler(bow::TextureMinificationFilter::Linear, bow::TextureMagnificationFilter::Linear, bow::TextureWrap::Clamp, bow::TextureWrap::Clamp);

    ///////////////////////////////////////////////////////////////////
    // RenderState

    bow::RenderState renderState;
    renderState.depthTest.Enabled = true;

    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        context->VClear(clearState);

        context->VSetViewport(bow::Viewport(0, 0, window->VGetWidth(), window->VGetHeight()));
        context->VDraw(bow::PrimitiveType::Triangles, vertexAttributeBindings, shaderProgram, renderState);

        context->VSwapBuffers();
    }

    OPTICK_SHUTDOWN();

    return 0;
}
