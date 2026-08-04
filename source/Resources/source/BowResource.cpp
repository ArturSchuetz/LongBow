#include "Resources/BowResource.h"



namespace bow
{

Resource::Resource(ResourceManager *creator, const std::string &name, ResourceHandle handle) : m_creator(creator), m_name(name), m_handle(handle), m_sizeInBytes(0), m_loadingState(LoadingState::LOADSTATE_UNLOADED), m_stateCount(0)
{
    FN("Resource::Resource");
}

Resource::~Resource()
{
    FN("Resource::~Resource");

    VUnload();
}

void Resource::VPrepare()
{
    FN("Resource::VPrepare");
    OPTICK_EVENT();

    // quick check that avoids any synchronisation
    if (m_loadingState != LoadingState::LOADSTATE_UNLOADED)
    {
        return;
    }

    VPrepareImpl();

    m_loadingState = LoadingState::LOADSTATE_PREPARED;
}

void Resource::VLoad()
{
    FN("Resource::VLoad");
    OPTICK_EVENT();

    if (m_loadingState != LoadingState::LOADSTATE_UNLOADED && m_loadingState != LoadingState::LOADSTATE_PREPARED)
    {
        return;
    }

    if (m_loadingState == LoadingState::LOADSTATE_UNLOADED)
    {
        VPrepareImpl();
    }

    VPreLoadImpl();

    VLoadImpl();

    VPostLoadImpl();

    m_loadingState = LoadingState::LOADSTATE_LOADED;
}

void Resource::VUnload()
{
    FN("Resource::VUnload");

    if (m_loadingState != LoadingState::LOADSTATE_LOADED && m_loadingState != LoadingState::LOADSTATE_PREPARED)
    {
        return;
    }

    if (m_loadingState == LoadingState::LOADSTATE_PREPARED)
    {
        VUnprepareImpl();
    }
    else if (m_loadingState == LoadingState::LOADSTATE_LOADED)
    {
        VPreUnloadImpl();

        VUnloadImpl();

        VPostUnloadImpl();

        VUnprepareImpl();
    }

    m_loadingState = LoadingState::LOADSTATE_UNLOADED;
}

void Resource::_dirtyState()
{
    FN("Resource::_dirtyState");

    // don't worry about threading here, count only ever increases so
    // doesn't matter if we get a lost increment (one is enough)
    ++m_stateCount;
}

} // namespace bow
