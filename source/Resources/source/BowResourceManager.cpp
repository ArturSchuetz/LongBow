#include "Resources/BowResourceManager.h"
#include "Resources/BowResource.h"

#include "CoreSystems/BowLogger.h"


namespace bow
{
static std::shared_ptr<ResourceManager> Instance;

ResourceManager::ResourceManager() : m_nextHandle(1), m_memoryUsage(0) { FN("ResourceManager::ResourceManager"); }

ResourceManager::~ResourceManager()
{
    FN("ResourceManager::~ResourceManager");

    VUnloadAll();
    VRemoveAll();
}

ResourcePtr ResourceManager::CreateResource(const std::string &name)
{
    FN("ResourceManager::CreateResource");
    OPTICK_EVENT();

    // Call creation implementation
    ResourcePtr ret = ResourcePtr(VCreateImpl(name, GetNextHandle()));

    VAddImpl(ret);

    return ret;
}

ResourcePtr ResourceManager::CreateOrRetrieve(const std::string &name)
{
    FN("ResourceManager::CreateOrRetrieve");
    OPTICK_EVENT();

    ResourcePtr res = VGetResource(name);
    bool created = false;
    if (!res)
    {
        created = true;
        res = CreateResource(name);
    }

    return res;
}

ResourcePtr ResourceManager::Prepare(const std::string &name)
{
    FN("ResourceManager::Prepare");
    OPTICK_EVENT();

    ResourcePtr r = CreateOrRetrieve(name);

    // ensure prepared
    r->VPrepare();

    return r;
}

ResourcePtr ResourceManager::Load(const std::string &name)
{
    FN("ResourceManager::Load");
    OPTICK_EVENT();

    ResourcePtr r = CreateOrRetrieve(name);

    // ensure loaded
    r->VLoad();

    return r;
}

void ResourceManager::Unload(const std::string &name)
{
    FN("ResourceManager::Unload");

    ResourcePtr res = VGetResource(name);
    if (res)
    {
        res->VUnload();
    }
}

void ResourceManager::Unload(ResourceHandle handle)
{
    FN("ResourceManager::Unload");

    ResourcePtr res = VGetResource(handle);

    if (res)
    {
        res->VUnload();
    }
}

void ResourceManager::VUnloadAll()
{
    FN("ResourceManager::VUnloadAll");

    auto iend = m_resourcesByName.end();
    for (auto i = m_resourcesByName.begin(); i != iend; ++i)
    {
        Resource *res = i->second.get();
        res->VUnload();
    }
}

void ResourceManager::Remove(const ResourcePtr &res)
{
    FN("ResourceManager::Remove");
    OPTICK_EVENT();

    VRemoveImpl(res);
}

void ResourceManager::Remove(const std::string &name)
{
    FN("ResourceManager::Remove");
    OPTICK_EVENT();

    ResourcePtr res = VGetResource(name);

    if (res)
    {
        VRemoveImpl(res);
    }
}

void ResourceManager::Remove(ResourceHandle handle)
{
    FN("ResourceManager::Remove");
    OPTICK_EVENT();

    ResourcePtr res = VGetResource(handle);

    if (res)
    {
        VRemoveImpl(res);
    }
}

void ResourceManager::VRemoveAll()
{
    FN("ResourceManager::VRemoveAll");

    m_resourcesByName.clear();
    m_resourcesByHandle.clear();
}

ResourcePtr ResourceManager::VGetResource(const std::string &name)
{
    FN("ResourceManager::VGetResource");

    auto it = m_resourcesByName.find(name);
    return it == m_resourcesByName.end() ? ResourcePtr() : it->second;
}

ResourcePtr ResourceManager::VGetResource(ResourceHandle handle)
{
    FN("ResourceManager::VGetResource");

    auto it = m_resourcesByHandle.find(handle);
    return it == m_resourcesByHandle.end() ? ResourcePtr() : it->second;
}

ResourceHandle ResourceManager::GetNextHandle()
{
    FN("ResourceManager::GetNextHandle");

    // This is an atomic operation and hence needs no locking
    return m_nextHandle++;
}

void ResourceManager::VAddImpl(ResourcePtr &res)
{
    FN("ResourceManager::VAddImpl");

    // Insert the name
    auto resultName = m_resourcesByName.insert(std::pair<std::string, ResourcePtr>(res->VGetName(), res));
    if (!resultName.second)
    {
        LOG_ERROR("ResourceManager::VAddImpl: Resource with the name %s already exists.", res->VGetName().c_str());
    }

    // Insert the handle
    auto resultHandle = m_resourcesByHandle.insert(std::pair<ResourceHandle, ResourcePtr>(res->VGetHandle(), res));
    if (!resultHandle.second)
    {
        LOG_ERROR("ResourceManager::VAddImpl: Resource with the handle %ld already exists.", (long)(res->VGetHandle()));
    }
}

void ResourceManager::VRemoveImpl(const ResourcePtr &res)
{
    FN("ResourceManager::VRemoveImpl");

    auto handleIt = m_resourcesByHandle.find(res->VGetHandle());
    if (handleIt != m_resourcesByHandle.end())
    {
        m_resourcesByHandle.erase(handleIt);
    }

    auto nameIt = m_resourcesByName.find(res->VGetName());
    if (nameIt != m_resourcesByName.end())
    {
        m_resourcesByName.erase(nameIt);
    }
}
} // namespace bow
