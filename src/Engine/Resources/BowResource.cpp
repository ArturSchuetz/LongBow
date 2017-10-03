#include "BowResources.h"
#include "BowResource.h"

namespace Bow {
	namespace Core {

		Resource::Resource(ResourceManager* creator, const std::string& name, ResourceHandle handle)
			: m_creator(creator)
			, m_name(name)
			, m_handle(handle)
			, m_sizeInBytes(0)
			, m_loadingState(LoadingState::LOADSTATE_UNLOADED)
			, m_stateCount(0)
		{
		}

		void Resource::VPrepare(void)
		{
			// quick check that avoids any synchronisation
			LoadingState old = m_loadingState;
			if (old != LoadingState::LOADSTATE_UNLOADED && old != LoadingState::LOADSTATE_PREPARING)
			{
				return;
			}

			VPrepareImpl();

			m_loadingState = LoadingState::LOADSTATE_PREPARED;
		}

		void Resource::VLoad(void)
		{
			LoadingState old = m_loadingState;

			if (old != LoadingState::LOADSTATE_UNLOADED && old != LoadingState::LOADSTATE_PREPARED && old != LoadingState::LOADSTATE_LOADING)
			{
				return;
			}

			if (old == LoadingState::LOADSTATE_UNLOADED)
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
			LoadingState old = m_loadingState;
			if (old != LoadingState::LOADSTATE_LOADED && old != LoadingState::LOADSTATE_PREPARED)
			{
				return;
			}

			if (old != LoadingState::LOADSTATE_UNLOADING)
			{
				return;
			}

			if (old == LoadingState::LOADSTATE_PREPARED) 
			{
				VUnprepareImpl();
			}
			else 
			{
				VPreUnloadImpl();
				VUnloadImpl();
				VPostUnloadImpl();
			}

			m_loadingState = LoadingState::LOADSTATE_UNLOADED;
		}

		void Resource::_dirtyState()
		{
			// don't worry about threading here, count only ever increases so 
			// doesn't matter if we get a lost increment (one is enough)
			++m_stateCount;
		}

		size_t Resource::VCalculateSize(void) const
		{
			size_t memSize = 0;
			memSize += sizeof(ResourceManager);
			memSize += sizeof(ResourceHandle);
			memSize += m_name.size() * sizeof(char);
			memSize += sizeof(size_t);
			memSize += sizeof(LoadingState);
			return memSize;
		}
	}
}