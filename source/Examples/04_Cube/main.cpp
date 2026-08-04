#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <CoreSystems/BowBasicTimer.h>
#include <CoreSystems/BowLogger.h>

#include <ExampleSupport/ExampleSupport.h>

#include <optick.h>

#include <iostream>

std::string glslVertexShader = R"(#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec4 varColor;

layout(push_constant) uniform PushConstants {
    mat4 modelViewProj;
} pushConstants;

void main() {
    gl_Position = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0) * pushConstants.modelViewProj;
    varColor = vec4(inColor, 1.0f);
}
)";

std::string glslFragmentShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 varColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = varColor;
}
)";

int main(int argc, char *argv[])
{
    FN("main");

    OPTICK_APP("Cube Sample");

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
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 600, "Cube", bow::WindowType::Windowed);
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

    bow::ClearState clearState;
    clearState.color = bow::ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

    ///////////////////////////////////////////////////////////////////
    // RenderState

    bow::RenderState renderState;
    renderState.faceCulling.Enabled = false;
    renderState.depthTest.Enabled = false;

    ///////////////////////////////////////////////////////////////////
    // Vertex Attribute Bindings

    static bow::Vector3<float> positions[] = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, 0.5f},
        {-0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},   {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f},   {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, 0.5f},
    };

    // fill buffer with informations
    bow::VertexBufferPtr positionsBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector3<float>) * 24);
    positionsBuffer->VCopyFromSystemMemory(positions, 0, sizeof(bow::Vector3<float>) * 24);
    bow::VertexBufferAttributePtr positionAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(positionsBuffer, bow::ComponentDatatype::Float, 3));

    static bow::Vector3<float> colors[] = {
        {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},
    };

    bow::VertexBufferPtr colorBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector3<float>) * 24);
    colorBuffer->VCopyFromSystemMemory(colors, 0, sizeof(bow::Vector3<float>) * 24);
    bow::VertexBufferAttributePtr colorsAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(colorBuffer, bow::ComponentDatatype::Float, 3));

    // static uint32_t indices[] = {1, 2, 0, 2, 3, 0, 6, 5, 4, 4, 7, 6, 8, 11, 10, 10, 9, 8, 12, 13, 14, 14, 15, 12, 18, 17, 16, 16, 19, 18, 22, 23, 20, 20, 21, 22};
    static uint32_t indices[] = {0, 2, 1, 0, 3, 2, 4, 5, 6, 6, 7, 4, 10, 11, 8, 8, 9, 10, 14, 13, 12, 12, 15, 14, 16, 17, 18, 18, 19, 16, 20, 23, 22, 22, 21, 20};
    bow::IndexBufferPtr indexBuffer = device->VCreateIndexBuffer(bow::BufferHint::StaticDraw, bow::IndexBufferDatatype::UnsignedInt32, sizeof(uint32_t) * 36);
    indexBuffer->VCopyFromSystemMemory(indices, 0, sizeof(uint32_t) * 36);

    // create VertexAttributeBindings
    bow::VertexAttributeBindingsPtr vertexAttributeBindings = context->VCreateVertexAttributeBindings();

    // connect buffer with location in shader
    vertexAttributeBindings->VSetIndexBuffer(indexBuffer);
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inPosition"), positionAttribute);
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inColor"), colorsAttribute);

    ///////////////////////////////////////////////////////////////////
    // Camera
    bow::Vector3<float> Position = bow::Vector3<float>(0.0f, 1.5f, -2.0f);
    bow::Vector3<float> LookAt = bow::Vector3<float>(0.0f, 0.0f, 0.0f);
    bow::Vector3<float> UpVector = bow::Vector3<float>(0.0f, 1.0f, 0.0f);

    bow::Camera camera(Position, LookAt, UpVector, window->VGetWidth(), window->VGetHeight());

    ///////////////////////////////////////////////////////////////////
    // Render Loop

    bow::Matrix3D<float> worldMat;
    worldMat.Translate(bow::Vector3<float>(0.0f, 0.0f, 0.0f));

    bow::BasicTimer timer;
    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        timer.Update();
        keyboard->VUpdate();

        if (keyboard->VIsPressed(bow::Key::K_ESCAPE))
            break;

        // Clear Backbuffer to our ClearState
        context->VClear(clearState);

        context->VSetViewport(bow::Viewport(0, 0, window->VGetWidth(), window->VGetHeight()));
        camera.SetResolution(window->VGetWidth(), window->VGetHeight());

        worldMat.RotateY(0.5f * timer.GetDelta());

        bow::Matrix4x4<float> mvp = (bow::Matrix4x4<float>)camera.CalculateWorldViewProjection(worldMat);
        shaderProgram->VSetPushConstants(bow::ShaderStage::Vertex, &mvp, 0, sizeof(bow::Matrix4x4<float>));

        context->VDraw(bow::PrimitiveType::Triangles, vertexAttributeBindings, shaderProgram, renderState);

        context->VSwapBuffers();
        LOG_UPDATE();
    }

    OPTICK_SHUTDOWN();

    return 0;
}
