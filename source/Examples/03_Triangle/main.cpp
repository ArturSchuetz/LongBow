#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <CoreSystems/BowBasicTimer.h>
#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <iostream>

std::string glslVertexShader = R"(#version 450
#extension GL_KHR_vulkan_glsl : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 varColor;

layout(push_constant) uniform PushConstants {
    vec2 offset;
} pushConstants;

void main() {
    gl_Position = vec4(inPosition.xy + pushConstants.offset, 0.5f, 1.0f);
    varColor = inColor;
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

int main(int /*argc*/, char * /*argv[]*/)
{
    FN("main");

    OPTICK_APP("Triangle Sample");

    ///////////////////////////////////////////////////////////////////
    // Creating Render Device and Window

    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(bow::RenderDeviceAPI::Vulkan);
    if (device == nullptr)
    {
        LOG_ERROR("Could not create device!");
        return -1;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(815, 645, "Triangle", bow::WindowType::Windowed);
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

    ///////////////////////////////////////////////////////////////////
    // Vertex Attribute Bindings

    float aspectRatio = static_cast<float>(800) / static_cast<float>(600);

    bow::Vector2<float> vertices[3];
    vertices[0] = bow::Vector2<float>(-0.25f, -0.25f * aspectRatio);
    vertices[1] = bow::Vector2<float>(0.25f, -0.25f * aspectRatio);
    vertices[2] = bow::Vector2<float>(0.0f, 0.25f * aspectRatio);

    // fill buffer with informations
    bow::VertexBufferPtr positionsBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector2<float>) * 3);
    positionsBuffer->VCopyFromSystemMemory(vertices, 0, sizeof(bow::Vector2<float>) * 3);
    bow::VertexBufferAttributePtr positionAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(positionsBuffer, bow::ComponentDatatype::Float, 2));

    bow::Vector4<float> colors[3];
    colors[0] = bow::Vector4<float>(1.0f, 0.0f, 0.0f, 1.0f);
    colors[1] = bow::Vector4<float>(0.0f, 1.0f, 0.0f, 1.0f);
    colors[2] = bow::Vector4<float>(0.0f, 0.0f, 1.0f, 1.0f);

    bow::VertexBufferPtr colorBuffer = device->VCreateVertexBuffer(bow::BufferHint::StaticDraw, sizeof(bow::Vector4<float>) * 3);
    colorBuffer->VCopyFromSystemMemory(colors, 0, sizeof(bow::Vector4<float>) * 3);
    bow::VertexBufferAttributePtr colorsAttribute = bow::VertexBufferAttributePtr(new bow::VertexBufferAttribute(colorBuffer, bow::ComponentDatatype::Float, 4));

    // create VertexAttributeBindings
    bow::VertexAttributeBindingsPtr vertexAttributeBindings = context->VCreateVertexAttributeBindings();

    // connect buffer with location in shader
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inPosition"), positionAttribute);
    vertexAttributeBindings->VSetAttribute(shaderProgram->VGetVertexAttribute("inColor"), colorsAttribute);

    ///////////////////////////////////////////////////////////////////
    // Render Loop

    bow::BasicTimer timer;
    bow::Vector2<float> offset = bow::Vector2<float>(0.0f, 0.0f);

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

        offset.x += 0.01f * timer.GetDelta();
        shaderProgram->VSetPushConstants(bow::ShaderStage::Vertex, &offset, 0, sizeof(bow::Vector2<float>));

        context->VDraw(bow::PrimitiveType::Triangles, vertexAttributeBindings, shaderProgram, renderState);

        context->VSwapBuffers();

        LOG_UPDATE();
    }

    OPTICK_SHUTDOWN();

    return 0;
}
