#include <InputDevice/BowInput.h>
#include <RenderDevice/BowRenderer.h>

#include <CoreSystems/BowLogger.h>

#include <ExampleSupport/ExampleSupport.h>

#include <optick.h>

#include <iostream>

const std::string computeShaderSource = R"(#version 450
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(set=0, binding=0) buffer InputBuffer {
    float inputData[];
};

layout(set=0, binding=1) buffer OutputBuffer {
    float outputData[];
};

void main() {
    uint index = gl_GlobalInvocationID.x;
    outputData[index] = inputData[index] * 2;  // Beispieloperation
})";

int main(int argc, char *argv[])
{
    FN("main");

    OPTICK_APP("Compute Shader Sample");

    // Creating Render Device
    bow::RenderDeviceAPI backend = bow::examples::SelectBackend(argc, argv);
    LOG_INFO("Render backend: %s", bow::examples::BackendName(backend));

    bow::RenderDevicePtr device = bow::RenderDeviceManager::GetInstance().CreateDevice(backend);
    if (device == nullptr)
    {
        LOG_ERROR("Could not create device!");
        return -1;
    }

    // Creating Window
    bow::GraphicsWindowPtr window = device->VCreateWindow(800, 600, "HelloWorld", bow::WindowType::Windowed);
    if (window == nullptr)
    {
        LOG_ERROR("Could not create window!");
        return -1;
    }

    ///////////////////////////////////////////////////////////////////
    // Shader

    bow::ComputeShaderProgramPtr computeShaderProgram = device->VCreateComputeShaderProgram(computeShaderSource);
    if (computeShaderProgram == nullptr)
    {
        LOG_ERROR("Could not create shader program!");
        return -1;
    }

    std::vector<float> inputData = {1.0f, 2.0f, 3.0f, 4.0f};
    bow::StorageBufferPtr storageInputBuffer = device->VCreateStorageBuffer(bow::BufferHint::StaticRead, inputData.size() * sizeof(float), inputData.data());
    bow::StorageBufferPtr storageOutputBuffer = device->VCreateStorageBuffer(bow::BufferHint::StreamCopy, inputData.size() * sizeof(float));

    bow::ShaderResourceBindingsPtr shaderResouceBindings = computeShaderProgram->VCreateComputeResourceBindingObjects();
    shaderResouceBindings->VSetBuffer("InputBuffer", storageInputBuffer);
    shaderResouceBindings->VSetBuffer("OutputBuffer", storageOutputBuffer);
    computeShaderProgram->VDispatch(shaderResouceBindings, inputData.size(), 1, 1);

    std::shared_ptr<void> calculatedData = storageOutputBuffer->VCopyToSystemMemory(0, inputData.size() * sizeof(float));

    for (size_t i = 0; i < inputData.size(); i++)
    {
        std::cout << "Input: " << inputData[i] << " Output: " << reinterpret_cast<float *>(calculatedData.get())[i] << std::endl;
    }

    OPTICK_SHUTDOWN();

    return 0;
}