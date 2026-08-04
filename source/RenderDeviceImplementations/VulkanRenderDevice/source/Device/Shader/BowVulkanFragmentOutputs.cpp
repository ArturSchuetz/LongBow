#include <VulkanRenderDevice/Device/Shader/BowVulkanFragmentOutputs.h>

#include <CoreSystems/BowLogger.h>

namespace bow
{

VulkanFragmentOutputs::VulkanFragmentOutputs() { FN("VulkanFragmentOutputs::VulkanFragmentOutputs"); }

VulkanFragmentOutputs::~VulkanFragmentOutputs() { FN("VulkanFragmentOutputs::~VulkanFragmentOutputs"); }

void VulkanFragmentOutputs::insert(std::pair<std::string, uint32_t> fragmentOutputLocation)
{
    FN("OGLFragmentOutputs::insert");

    m_outputs.insert(fragmentOutputLocation);
}

int VulkanFragmentOutputs::operator[](std::string name) const
{
    FN("OGLFragmentOutputs::operator[]");

    int i = -1;
    for (auto it = m_outputs.begin(); it != m_outputs.end(); ++it)
    {
        if (it->first == name)
        {
            i = it->second;
            break;
        }
    }

    if (i == -1)
    {
        LOG_ASSERT(i != -1, "Fragment Output Key does not exist.");
        return -1;
    }

    return i;
}

} // namespace bow