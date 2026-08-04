#include "FirstPersonCamera.h"

#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <Resources/BowResources.h>

#include <CoreSystems/BowCoreSystems.h>

#include <ExampleSupport/ExampleSupport.h>

#include <optick.h>

#include <iostream>

std::string glslVertexShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 var_lightDir;
layout(location = 1) out vec3 var_Normal;
layout(location = 2) out vec2 var_textureCoord;

layout(push_constant) uniform PushConstants {
    mat4 u_ModelViewProj;
    mat4 u_ModelView;
} pushConstants;

layout(set = 0, binding = 0) uniform MVP {
    mat4 u_View;
} mvp;

void main()
{
    gl_Position = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0) * pushConstants.u_ModelViewProj;

    vec3 light_position = (vec4(0.0, 1000.0, 0.0, 1.0) * mvp.u_View).xyz;
    var_lightDir = normalize(light_position - (vec4(inPosition, 1.0) * pushConstants.u_ModelView).xyz);

    var_Normal = inNormal * mat3(pushConstants.u_ModelView);
    var_textureCoord = inTexCoord;
})";

std::string glslFragmentShader = R"(#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 var_lightDir;
layout(location = 1) in vec3 var_Normal;
layout(location = 2) in vec2 var_textureCoord;

layout(set = 1, binding = 0) uniform sampler2D alphaTex;
layout(set = 1, binding = 1) uniform sampler2D diffuseTex;

layout(set = 2, binding = 2) uniform Material {
    vec4 u_diffuseColor;
} material;

layout(location = 0) out vec4 out_Color;

void main(void)
{
    float alphaColor = texture(alphaTex, vec2(var_textureCoord.x, -var_textureCoord.y)).r;
    if(alphaColor < 0.5 && material.u_diffuseColor.a > 0.5)
    {
        discard;
    }

    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 N = normalize(var_Normal);
    vec3 L = normalize(var_lightDir);

    vec3 diffuse = max(dot(N, L), 0.0) * texture(diffuseTex, vec2(var_textureCoord.x, -var_textureCoord.y)).rgb * lightColor;

    out_Color = vec4(diffuse.rgb, 1.0);
})";

struct PushConstants
{
    bow::Matrix4x4<float> u_ModelViewProj = bow::Matrix4x4<float>();
    bow::Matrix4x4<float> u_ModelView = bow::Matrix4x4<float>();
};

struct MVP
{
    bow::Matrix4x4<float> u_View = bow::Matrix4x4<float>();
};

struct Material
{
    bow::Vector4<float> u_diffuseColor = bow::Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f);
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

    OPTICK_APP("Mesh Rendering Sample");


    // Creating Render Device
    bow::RenderDeviceAPI backend = bow::examples::SelectBackend(argc, argv);
    LOG_INFO("Render backend: %s", bow::examples::BackendName(backend));

    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(backend);
    if (device == nullptr)
    {
        return 0;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 800, "Mesh Rendering Sample", bow::WindowType::Windowed);
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
    // Vertex Array from Mesh

    bow::MeshPtr mesh = bow::MeshManager::GetInstance().Load(objFilePath);

    bow::MeshAttribute meshAttr = mesh->CreateAttribute("inPosition", "inNormal", "inTexCoord");
    bow::VertexAttributeBindingsPtr meshVertexAttributeBindings = context->VCreateVertexAttributeBindings(meshAttr, shaderProgram->VGetVertexAttributes(), bow::BufferHint::StaticDraw);

    std::vector<bow::SubMesh *> subMeshes = mesh->GetSubMeshes();

    std::unordered_map<std::string, bow::Texture2DPtr> materialDiffuseTextures;
    std::unordered_map<std::string, bow::Texture2DPtr> materialAlphaTextures;

    std::vector<std::string> materialFiles = mesh->GetMaterialFiles();

    bow::TextureSamplerPtr samplerDiffuse = device->VCreateTexture2DSampler(bow::TextureMinificationFilter::Linear, bow::TextureMagnificationFilter::Linear, bow::TextureWrap::Repeat, bow::TextureWrap::Repeat);
    bow::TextureSamplerPtr samplerAlpha = device->VCreateTexture2DSampler(bow::TextureMinificationFilter::Nearest, bow::TextureMagnificationFilter::Nearest, bow::TextureWrap::Repeat, bow::TextureWrap::Repeat);

    std::vector<bow::Material *> materials(subMeshes.size());
    std::vector<bow::UniformBufferPtr> materialUniformBufferObjects(subMeshes.size());                                   // One Material UBO for each SubMeshes
    bow::UniformBufferPtr uniformBufferObject = device->VCreateUniformBuffer(bow::BufferHint::DynamicDraw, sizeof(MVP)); // One MVP UBO for each Mesh

    std::vector<bow::ShaderResourceBindingsPtr> shaderResourceBindings(subMeshes.size());
    {
        OPTICK_EVENT("Parse Materials for Sub Meshes");

        for (size_t i = 0; i < subMeshes.size(); i++)
        {
            shaderResourceBindings[i] = shaderProgram->VCreateResourceBindingObjects();
            materialUniformBufferObjects[i] = device->VCreateUniformBuffer(bow::BufferHint::StaticDraw, sizeof(Material));
            shaderResourceBindings[i]->VSetBuffer("material", materialUniformBufferObjects[i]);
            shaderResourceBindings[i]->VSetBuffer("mvp", uniformBufferObject);
        }

        for (unsigned int i = 0; i < materialFiles.size(); i++)
        {
            bow::MaterialCollectionPtr materialCollection = bow::MaterialManager::GetInstance().Load(materialFiles[i]);
            if (materialCollection != nullptr)
            {
                std::size_t foundPos = materialCollection->VGetName().find_last_of("/");
                std::string filePath = "";
                if (foundPos >= 0)
                {
                    filePath = materialCollection->VGetName().substr(0, foundPos + 1);
                }

                for (unsigned int j = 0; j < subMeshes.size(); j++)
                {
                    materials[j] = materialCollection->GetMaterial(subMeshes[j]->GetMaterialName());
                    if (materials[j] != nullptr)
                    {
                        if (materialDiffuseTextures.find(materials[j]->diffuse_texname) == materialDiffuseTextures.end())
                        {
                            LOG_INFO("Loading Texture: %s", std::string(filePath + materials[j]->diffuse_texname).c_str());

                            bow::ImagePtr diffuseImage = bow::ImageManager::GetInstance().Load(filePath + materials[j]->diffuse_texname);
                            bow::Texture2DPtr diffuseTexture = device->VCreateTexture2D(diffuseImage);
                            if (diffuseTexture != nullptr)
                            {
                                materialDiffuseTextures[materials[j]->diffuse_texname] = diffuseTexture;
                                shaderResourceBindings[j]->VSetTexture("diffuseTex", diffuseTexture, samplerDiffuse);
                            }
                        }
                        else
                        {
                            shaderResourceBindings[j]->VSetTexture("diffuseTex", materialDiffuseTextures[materials[j]->diffuse_texname], samplerDiffuse);
                        }

                        Material material;
                        if (materialAlphaTextures.find(materials[j]->alpha_texname) == materialAlphaTextures.end())
                        {
                            LOG_INFO("Loading Texture: %s", std::string(filePath + materials[j]->alpha_texname).c_str());

                            bow::ImagePtr alphaMask = bow::ImageManager::GetInstance().Load(filePath + materials[j]->alpha_texname);
                            bow::Texture2DPtr alphaTexture = device->VCreateTexture2D(alphaMask);
                            if (alphaTexture != nullptr)
                            {
                                materialAlphaTextures[materials[j]->alpha_texname] = alphaTexture;
                                shaderResourceBindings[j]->VSetTexture("alphaTex", alphaTexture, samplerAlpha);
                                material.u_diffuseColor = bow::Vector4<float>(materials[j]->diffuse[0], materials[j]->diffuse[1], materials[j]->diffuse[2], 1.0f);
                            }
                            else
                            {
                                material.u_diffuseColor = bow::Vector4<float>(materials[j]->diffuse[0], materials[j]->diffuse[1], materials[j]->diffuse[2], 0.0f);
                            }
                        }
                        else
                        {
                            shaderResourceBindings[j]->VSetTexture("alphaTex", materialAlphaTextures[materials[j]->alpha_texname], samplerAlpha);
                            material.u_diffuseColor = bow::Vector4<float>(materials[j]->diffuse[0], materials[j]->diffuse[1], materials[j]->diffuse[2], 1.0f);
                        }
                        materialUniformBufferObjects[j]->VCopyFromSystemMemory(&material, 0, sizeof(Material));
                    }
                }
            }
        }
    }

    bow::Vector3<float> bbox_min;
    bow::Vector3<float> bbox_max;
    mesh->GetBoundigBox(bbox_min, bbox_max);

    ///////////////////////////////////////////////////////////////////
    // ClearState and Color

    bow::ClearState clearState;
    clearState.color = bow::ColorRGBA(0.392f, 0.584f, 0.929f, 1.0f);

    ///////////////////////////////////////////////////////////////////
    // FirstPersonCamera

    bow::Vector3<float> Position = (bbox_min + bbox_max) * 0.5f;
    bow::Vector3<float> LookAt = Position + bow::Vector3<float>(1.0f, 0.0f, 0.0f);
    bow::Vector3<float> UpVector = bow::Vector3<float>(0.0f, 1.0f, 0.0f);

    FirstPersonCamera fpCamera = FirstPersonCamera(Position, LookAt, UpVector, window->VGetWidth(), window->VGetHeight());
    fpCamera.SetClippingPlanes(1.0, 10000.0);

    ///////////////////////////////////////////////////////////////////
    // Input

    bow::KeyboardPtr keyboard = bow::InputDeviceManager::GetInstance().CreateKeyboardObject(window);
    if (keyboard == nullptr)
    {
        return false;
    }

    bow::MousePtr mouse = bow::InputDeviceManager::GetInstance().CreateMouseObject(window);
    if (mouse == nullptr)
    {
        return false;
    }

    ///////////////////////////////////////////////////////////////////
    // RenderState

    bow::RenderState renderState;
    renderState.rasterizationMode = bow::RasterizationMode::Fill;
    renderState.faceCulling.Enabled = false;

    ///////////////////////////////////////////////////////////////////
    // Gameloop

    bow::Matrix3D<float> worldMat;
    worldMat.Translate(bow::Vector3<float>(0.0f, 0.0f, 0.0f));

    bow::BasicTimer timer;
    float m_moveSpeed;
    bow::Vector3<long> lastCursorPosition;

    bow::Matrix3D<float> identityMat;
    identityMat.SetIdentity();

    MVP mvp = {};
    mvp.u_View = (bow::Matrix4x4<float>)fpCamera.CalculateView();
    uniformBufferObject->VCopyFromSystemMemory(&mvp, 0, sizeof(MVP));

    PushConstants pushConstants = {};
    while (!window->VShouldClose())
    {
        OPTICK_FRAME("MainThread");

        {
            OPTICK_CATEGORY("HandleInput", Optick::Category::Input);
            timer.Update();

            keyboard->VUpdate();
            mouse->VUpdate();

            m_moveSpeed = 10000.0;

            bool cameraUpdated = false;
            if (keyboard->VIsPressed(bow::Key::K_W))
            {
                if (keyboard->VIsPressed(bow::Key::K_LEFT_SHIFT))
                {
                    fpCamera.MoveForward(m_moveSpeed * (float)timer.GetDelta() * 4.0f);
                }
                else
                {
                    fpCamera.MoveForward(m_moveSpeed * (float)timer.GetDelta());
                }
                cameraUpdated = true;
            }

            if (keyboard->VIsPressed(bow::Key::K_S))
            {
                fpCamera.MoveBackward(m_moveSpeed * (float)timer.GetDelta());
                cameraUpdated = true;
            }

            if (keyboard->VIsPressed(bow::Key::K_D))
            {
                fpCamera.MoveRight(m_moveSpeed * (float)timer.GetDelta());
                cameraUpdated = true;
            }

            if (keyboard->VIsPressed(bow::Key::K_A))
            {
                fpCamera.MoveLeft(m_moveSpeed * (float)timer.GetDelta());
                cameraUpdated = true;
            }

            if (keyboard->VIsPressed(bow::Key::K_SPACE))
            {
                fpCamera.MoveUp(m_moveSpeed * (float)timer.GetDelta());
                cameraUpdated = true;
            }

            if (keyboard->VIsPressed(bow::Key::K_LEFT_CONTROL))
            {
                fpCamera.MoveDown(m_moveSpeed * (float)timer.GetDelta());
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

            if (cameraUpdated)
            {
                mvp.u_View = (bow::Matrix4x4<float>)fpCamera.CalculateView();
                uniformBufferObject->VCopyFromSystemMemory(&mvp, 0, sizeof(MVP));
            }

            lastCursorPosition = mouse->VGetAbsolutePosition();
        }

        // =======================================================

        {
            OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

            context->VClear(clearState);

            context->VSetViewport(bow::Viewport(0, 0, std::max(window->VGetWidth(), 1), std::max(window->VGetHeight(), 1)));

            fpCamera.SetResolution(window->VGetWidth(), window->VGetHeight());

            pushConstants.u_ModelViewProj = (bow::Matrix4x4<float>)fpCamera.CalculateWorldViewProjection(worldMat);
            pushConstants.u_ModelView = (bow::Matrix4x4<float>)fpCamera.CalculateWorldView(worldMat);
            shaderProgram->VSetPushConstants("pushConstants", &pushConstants, 0, sizeof(pushConstants));

            for (unsigned int i = 0; i < subMeshes.size(); i++)
            {
                context->VDraw(bow::PrimitiveType::Triangles, subMeshes[i]->GetStartIndex(), subMeshes[i]->GetNumIndices(), meshVertexAttributeBindings, shaderResourceBindings[i], shaderProgram, renderState);
            }

            context->VSwapBuffers();
        }

        // =======================================================

        LOG_UPDATE();
    }

    OPTICK_SHUTDOWN();

    return 0;
}
