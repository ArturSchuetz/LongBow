#pragma once
#include <VulkanRenderDevice/BowVulkanRenderDevicePredeclares.h>
#include <VulkanRenderDevice/VulkanRenderDevice_api.h>

#include <VulkanRenderDevice/VulkanFunctions.h>

#include <RenderDevice/Device/Shader/IBowFragmentOutputs.h>

#include <map>

namespace bow
{
class VulkanFragmentOutputs : public IFragmentOutputs
{
  public:
    VulkanFragmentOutputs();
    ~VulkanFragmentOutputs();

    void insert(std::pair<std::string, uint32_t> fragmentOutputLocation);

    int operator[](std::string name) const;

  private:
    std::map<std::string, uint32_t> m_outputs;
};

} // namespace bow