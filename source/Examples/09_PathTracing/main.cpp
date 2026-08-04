#include "FirstPersonCamera.h"

#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <Resources/BowResources.h>

#include <CoreSystems/BowCoreSystems.h>

#include <ExampleSupport/ExampleSupport.h>

#include <optick.h>

#include <iostream>

std::string glslRayGenShader = R"(#version 460
#extension GL_EXT_ray_tracing : require

layout(set = 0, binding = 0) uniform accelerationStructureEXT topLevelAS;
layout(set = 0, binding = 1, rgba8) uniform image2D outputImage;

layout(set = 0, binding = 2) uniform CameraUBO {
    mat4 viewInverse;
    mat4 projInverse;
} camera;

layout(location = 0) rayPayloadEXT vec3 hitValue;

void main()
{
    const vec2 pixelCenter = vec2(gl_LaunchIDEXT.xy) + vec2(0.5);
    const vec2 inUV = pixelCenter / vec2(gl_LaunchSizeEXT.xy);
    vec2 d = inUV * 2.0 - 1.0;
    d.y = -d.y;

    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0) * camera.viewInverse;
    vec4 target = vec4(d.x, d.y, 1.0, 1.0) * camera.projInverse;
    vec4 direction = vec4(normalize(target.xyz), 0.0) * camera.viewInverse;

    float tmin = 0.001;
    float tmax = 10000.0;

    hitValue = vec3(0.0);

    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xFF, 0, 0, 0, origin.xyz, tmin, direction.xyz, tmax, 0);

    imageStore(outputImage, ivec2(gl_LaunchIDEXT.xy), vec4(hitValue, 1.0));
}
)";

std::string glslClosestHitShader = R"(#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_ray_tracing_position_fetch : require

layout(location = 0) rayPayloadInEXT vec3 hitValue;

hitAttributeEXT vec2 attribs;

void main()
{
    // Fetch triangle vertex positions to compute flat normal
    vec3 v0 = gl_HitTriangleVertexPositionsEXT[0];
    vec3 v1 = gl_HitTriangleVertexPositionsEXT[1];
    vec3 v2 = gl_HitTriangleVertexPositionsEXT[2];

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
    vec3 N = normalize(cross(e1, e2));

    // Flip normal if it faces away from the ray
    if (dot(N, gl_WorldRayDirectionEXT) > 0.0)
        N = -N;

    // Simple directional light
    vec3 lightDir = normalize(vec3(1.0, 1.0, -1.0));
    float NdotL = max(dot(N, lightDir), 0.0);

    vec3 baseColor = vec3(0.8, 0.8, 0.8);
    vec3 ambient = 0.2 * baseColor;

    hitValue = ambient + baseColor * NdotL;
}
)";

std::string glslMissShader = R"(#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitValue;

void main()
{
    // Sky gradient
    vec3 direction = normalize(gl_WorldRayDirectionEXT);
    float t = 0.5 * (direction.y + 1.0);
    hitValue = mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), t);
}
)";

struct CameraUBO
{
    bow::Matrix4x4<float> viewInverse;
    bow::Matrix4x4<float> projInverse;
};

int main(int argc, char *argv[])
{
    const char *meshArgument = bow::examples::PositionalArgument(argc, argv, 0);
    if (meshArgument == nullptr)
    {
        std::cerr << "Usage: " << argv[0] << " <path to .obj file> [--backend opengl|directx12|vulkan]" << std::endl;
        return -1;
    }

    std::string objFilePath = meshArgument;

    FN("main");

    OPTICK_APP("Ray Tracing Sample");

    // Creating Render Device
    bow::RenderDeviceAPI backend = bow::examples::SelectBackend(argc, argv);
    LOG_INFO("Render backend: %s", bow::examples::BackendName(backend));

    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(backend);
    if (device == nullptr)
    {
        return -1;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 800, "Ray Tracing Sample", bow::WindowType::Windowed);
    if (window == nullptr)
    {
        return -1;
    }

    bow::RenderContextPtr context = window->VGetContext();

    std::unique_ptr<bow::IRayTracingShaderProgram> shaderProgram = device->VCreateRayTracingShaderProgram(glslRayGenShader, "", glslClosestHitShader, glslMissShader, "", "");
    if (shaderProgram == nullptr)
    {
        LOG_ERROR("Could not create shader program!");
        return -1;
    }

    // Register descriptor bindings matching the GLSL layout declarations
    shaderProgram->VAddDescriptorBinding(0, 0, "topLevelAS", bow::RTDescriptorType::AccelerationStructure);
    shaderProgram->VAddDescriptorBinding(0, 1, "outputImage", bow::RTDescriptorType::StorageImage);
    shaderProgram->VAddDescriptorBinding(0, 2, "camera", bow::RTDescriptorType::UniformBuffer);
    if (!shaderProgram->VBuild())
    {
        LOG_ERROR("Could not build RT pipeline!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Load Mesh and build acceleration structures

    bow::MeshPtr mesh = bow::MeshManager::GetInstance().Load(objFilePath);

    std::vector<std::unique_ptr<bow::IBottomLevelAccelerationStructure>> bottomLevelASs;
    std::unique_ptr<bow::IBottomLevelAccelerationStructure> bottomLevelAS = device->VCreateBottomLevelAccelerationStructure(mesh);
    if (bottomLevelAS == nullptr)
    {
        LOG_ERROR("Could not create BLAS!");
        return -1;
    }
    bottomLevelASs.push_back(std::move(bottomLevelAS));

    std::unique_ptr<bow::ITopLevelAccelerationStructure> topLevelAS = device->VCreateTopLevelAccelerationStructure(bottomLevelASs);
    if (topLevelAS == nullptr)
    {
        LOG_ERROR("Could not create TLAS!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Storage image for ray tracing output

    bow::Texture2DPtr storageImage = device->VCreateTexture2D(bow::Texture2DDescription(window->VGetWidth(), window->VGetHeight(), bow::TextureFormat::RedGreenBlueAlpha8, false));

    ///////////////////////////////////////////////////////////////////
    // Camera UBO

    bow::UniformBufferPtr cameraUBO = device->VCreateUniformBuffer(bow::BufferHint::DynamicDraw, sizeof(CameraUBO));

    ///////////////////////////////////////////////////////////////////
    // Resource bindings

    bow::ShaderResourceBindingsPtr resourceBindings = shaderProgram->VCreateResourceBindingObjects();
    resourceBindings->VSetAccelerationStructure("topLevelAS", topLevelAS.get());
    resourceBindings->VSetStorageImage("outputImage", storageImage);
    resourceBindings->VSetBuffer("camera", cameraUBO);

    ///////////////////////////////////////////////////////////////////
    // Camera Setup

    bow::Vector3<float> bbox_min;
    bow::Vector3<float> bbox_max;
    mesh->GetBoundigBox(bbox_min, bbox_max);

    bow::Vector3<float> center = (bbox_min + bbox_max) * 0.5f;
    float extent = (bbox_max - bbox_min).Length();
    bow::Vector3<float> cameraPos = center + bow::Vector3<float>(0.0f, extent * 0.15f, extent * 0.4f);

    FirstPersonCamera fpCamera(cameraPos, center, bow::Vector3<double>(0.0, 1.0, 0.0), window->VGetWidth(), window->VGetHeight());
    fpCamera.SetFOV(90.0f);
    fpCamera.SetClippingPlanes(0.01f, extent * 10.0f);

    ///////////////////////////////////////////////////////////////////
    // Input

    bow::KeyboardPtr keyboard = nullptr;
    bow::MousePtr mouse = nullptr;
    try
    {
        keyboard = bow::InputDeviceManager::GetInstance().CreateKeyboardObject(window);
        mouse = bow::InputDeviceManager::GetInstance().CreateMouseObject(window);
    }
    catch (...)
    {
        LOG_ERROR("Input device creation failed - running without input");
    }
    if (keyboard == nullptr || mouse == nullptr)
    {
        LOG_ERROR("Input devices not available - camera will be static");
    }

    ///////////////////////////////////////////////////////////////////
    // Gameloop

    bow::BasicTimer timer;
    float moveSpeed = extent * 0.5f;
    bow::Vector3<long> lastCursorPosition;

    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        {
            OPTICK_CATEGORY("HandleInput", Optick::Category::Input);
            timer.Update();

            if (keyboard != nullptr && mouse != nullptr)
            {
                keyboard->VUpdate();
                mouse->VUpdate();

                bool cameraUpdated = false;
                if (keyboard->VIsPressed(bow::Key::K_W))
                {
                    fpCamera.MoveForward(moveSpeed * (float)timer.GetDelta() * (keyboard->VIsPressed(bow::Key::K_LEFT_SHIFT) ? 4.0f : 1.0f));
                    cameraUpdated = true;
                }
                if (keyboard->VIsPressed(bow::Key::K_S))
                {
                    fpCamera.MoveBackward(moveSpeed * (float)timer.GetDelta());
                    cameraUpdated = true;
                }
                if (keyboard->VIsPressed(bow::Key::K_D))
                {
                    fpCamera.MoveRight(moveSpeed * (float)timer.GetDelta());
                    cameraUpdated = true;
                }
                if (keyboard->VIsPressed(bow::Key::K_A))
                {
                    fpCamera.MoveLeft(moveSpeed * (float)timer.GetDelta());
                    cameraUpdated = true;
                }
                if (keyboard->VIsPressed(bow::Key::K_SPACE))
                {
                    fpCamera.MoveUp(moveSpeed * (float)timer.GetDelta());
                    cameraUpdated = true;
                }
                if (keyboard->VIsPressed(bow::Key::K_LEFT_CONTROL))
                {
                    fpCamera.MoveDown(moveSpeed * (float)timer.GetDelta());
                    cameraUpdated = true;
                }
                if (mouse->VIsPressed(bow::MouseButton::MOFS_BUTTON1))
                {
                    window->VHideCursor();
                    bow::Vector3<long> moveVec = mouse->VGetRelativePosition();
                    fpCamera.rotate((float)moveVec.x, (float)moveVec.y);
                    mouse->VSetCursorPosition(lastCursorPosition.x, lastCursorPosition.y);
                    cameraUpdated = true;
                }
                else
                {
                    window->VShowCursor();
                }

                lastCursorPosition = mouse->VGetAbsolutePosition();

                if (cameraUpdated)
                {
                    fpCamera.SetResolution(window->VGetWidth(), window->VGetHeight());
                }
            }
        }

        // =======================================================

        {
            OPTICK_CATEGORY("RayTrace", Optick::Category::Rendering);

            // Update camera UBO
            CameraUBO camData = {};
            bow::Matrix3D<float> view = fpCamera.CalculateView();
            bow::Matrix4x4<float> proj = fpCamera.CalculateProjection();

            camData.viewInverse = (bow::Matrix4x4<float>)view.Inverse();
            camData.projInverse = proj.Inverse();
            cameraUBO->VCopyFromSystemMemory(&camData, 0, sizeof(CameraUBO));

            uint32_t width = std::max(window->VGetWidth(), 1);
            uint32_t height = std::max(window->VGetHeight(), 1);

            // Ray trace (no VClear -- vkCmdTraceRaysKHR must be outside a render pass)
            context->VTraceRays(shaderProgram.get(), resourceBindings, storageImage, width, height);

            context->VSwapBuffers();
        }

        LOG_UPDATE();
    }

    OPTICK_SHUTDOWN();

    return 0;
}
