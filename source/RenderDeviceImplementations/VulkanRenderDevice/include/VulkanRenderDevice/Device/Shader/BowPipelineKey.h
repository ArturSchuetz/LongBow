#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/BowRenderState.h>
#include <RenderDevice/Device/IBowRenderContext.h>

namespace bow
{

struct PipelineKey
{
    PipelineKey() : primitiveType(PrimitiveType::Triangles), renderState(), renderPass(VK_NULL_HANDLE) {}
    PipelineKey(const PrimitiveType &primitiveType, const RenderState &renderState, const VkRenderPass &renderPass) : primitiveType(primitiveType), renderState(renderState), renderPass(renderPass) {}

    PrimitiveType primitiveType;
    RenderState renderState;
    VkRenderPass renderPass;

    bool operator==(const PipelineKey &other) const { return primitiveType == other.primitiveType && renderState == other.renderState && renderPass == other.renderPass; }
};

} // namespace bow

// Move this to a separate header file if needed to avoid multiple definitions
namespace std
{

template <> struct hash<bow::PipelineKey>
{
    size_t operator()(const bow::PipelineKey &key) const
    {
        size_t hash = std::hash<bow::PrimitiveType>()(key.primitiveType);
        hash ^= std::hash<bow::RenderState>()(key.renderState) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<VkRenderPass>()(key.renderPass) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

} // namespace std
