#include "BowResources.h"
#include "BowResource.h"

namespace bow {
	
	Resource::Resource(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: m_creator(creator)
		, m_name(name)
		, m_handle(handle)
		, m_sizeInBytes(0)
		, m_loadingState(LoadingState::LOADSTATE_UNLOADED)
		, m_stateCount(0)
	{
	}

	Resource::~Resource() 
	{
		VUnload();
	}

	void Resource::VPrepare(void)
	{
		// quick check that avoids any synchronisation
		if (m_loadingState != LoadingState::LOADSTATE_UNLOADED)
		{
			return;
		}

		VPrepareImpl();

		m_loadingState = LoadingState::LOADSTATE_PREPARED;
	}

	void Resource::VLoad(void)
	{
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

	void Resource::VUnload(void)
	{
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
		// don't worry about threading here, count only ever increases so 
		// doesn't matter if we get a lost increment (one is enough)
		++m_stateCount;
	}
}
