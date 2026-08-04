#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include <CoreSystems/BowLogger.h>

#include <optick.h>

#include <string>

namespace bow
{
class RESOURCES_API Resource
{
  public:
    /// Enum identifying the loading state of the resource
    enum class LoadingState : char
    {
        LOADSTATE_UNLOADED = 0,
        LOADSTATE_LOADED = 2,
        LOADSTATE_PREPARED = 4
    };

    Resource(ResourceManager *creator, const std::string &name, ResourceHandle handle);
    virtual ~Resource();

    virtual void VPrepare();
    virtual void VLoad();
    virtual void VUnload();

    virtual ResourceManager *VGetCreator()
    {
        FN("Resource::VGetCreator");

        return m_creator;
    }

    virtual const std::string &VGetName() const
    {
        FN("Resource::VGetName");

        return m_name;
    }

    virtual ResourceHandle VGetHandle() const
    {
        FN("Resource::VGetHandle");

        return m_handle;
    }

    virtual size_t VGetSizeInBytes() const
    {
        FN("Resource::VGetSizeInBytes");

        return m_sizeInBytes;
    }

    virtual LoadingState VGetLoadingState() const
    {
        FN("Resource::VGetLoadingState");

        return m_loadingState;
    }

    virtual bool VIsPrepared() const
    {
        FN("Resource::VIsPrepared");

        return (m_loadingState == LoadingState::LOADSTATE_PREPARED);
    }

    virtual bool VIsLoaded() const
    {
        FN("Resource::VIsLoaded");

        return (m_loadingState == LoadingState::LOADSTATE_LOADED);
    }

    virtual void VSetToLoaded()
    {
        FN("Resource::VSetToLoaded");

        m_loadingState = LoadingState::LOADSTATE_LOADED;
    }

    virtual size_t getStateCount() const
    {
        FN("Resource::getStateCount");

        return m_stateCount;
    }

    virtual void _dirtyState();

  protected:
    Resource() : m_creator(nullptr), m_handle(0), m_sizeInBytes(0), m_loadingState(LoadingState::LOADSTATE_UNLOADED), m_stateCount(0) { FN("Resource::Resource"); }

    virtual void VPreLoadImpl() { FN("Resource::VPreLoadImpl"); }

    virtual void VPostLoadImpl() { FN("Resource::VPostLoadImpl"); }

    virtual void VPreUnloadImpl() { FN("Resource::VPreUnloadImpl"); }

    virtual void VPostUnloadImpl() { FN("Resource::VPostUnloadImpl"); }

    virtual void VPrepareImpl() { FN("Resource::VPrepareImpl"); }

    virtual void VUnprepareImpl() { FN("Resource::VUnprepareImpl"); }

    virtual void VLoadImpl() = 0;

    virtual void VUnloadImpl() = 0;

    ResourceManager *m_creator;  /// Creator
    std::string m_name;          /// Unique name of the resource
    ResourceHandle m_handle;     /// Numeric handle for more efficient look up than name
    size_t m_sizeInBytes;        /// The size of the resource in bytes
    LoadingState m_loadingState; /// Is the resource currently loaded?
    size_t m_stateCount;         /// State count, the number of times this resource has
                                 /// changed state

  private:
    // you shall not copy
    Resource(const Resource &other) = delete;
    Resource &operator=(const Resource &other) = delete;
};

} // namespace bow
