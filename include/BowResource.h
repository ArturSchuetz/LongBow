#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

namespace Bow {
	namespace Core {

		class Resource
		{
		public:
			/// Enum identifying the loading state of the resource
			enum class LoadingState : char
			{
				LOADSTATE_UNLOADED = 0,
				LOADSTATE_LOADING = 1,
				LOADSTATE_LOADED = 2,
				LOADSTATE_UNLOADING = 3,
				LOADSTATE_PREPARED = 4,
				LOADSTATE_PREPARING = 5
			};

			Resource(ResourceManager* creator, const std::string& name, ResourceHandle handle);
			virtual ~Resource() {}

			/** Prepares the resource for load, if it is not already.  One can call prepare()
				before load(), but this is not required as load() will call prepare()
				itself, if needed.  When OGRE_THREAD_SUPPORT==1 both load() and prepare()
				are thread-safe.  When OGRE_THREAD_SUPPORT==2 however, only prepare()
				is thread-safe.  The reason for this function is to allow a background
				thread to do some of the loading work, without requiring the whole render
				system to be thread-safe.  The background thread would call
				prepare() while the main render loop would later call load().  So long as
				prepare() remains thread-safe, subclasses can arbitrarily split the work of
				loading a resource between load() and prepare().  It is best to try and
				do as much work in prepare(), however, since this will leave less work for
				the main render thread to do and thus increase FPS.
			*/
			virtual void VPrepare(void);

			/** Loads the resource, if it is not already.
			@remarks
				If the resource is loaded from a file, loading is automatic. If not,
				if for example this resource gained it's data from procedural calls
				rather than loading from a file, then this resource will not reload
				on it's own.
			*/
			virtual void VLoad(void);

			/** Unloads the resource; this is not permanent, the resource can be
				reloaded later if required.
			*/
			virtual void VUnload(void);

			/// Gets the manager which created this resource
			virtual ResourceManager* VGetCreator(void) { return m_creator; }

			/** Gets resource name.
			*/
			virtual const std::string& VGetName(void) const { return m_name; }
			virtual ResourceHandle VGetHandle(void) const { return m_handle; }

			/** Retrieves info about the size of the resource.
			*/
			virtual size_t VGetSize(void) const { return m_sizeInBytes; }

			virtual LoadingState VGetLoadingState() const { return m_loadingState; }
			virtual bool VIsPrepared(void) const { return (m_loadingState == LoadingState::LOADSTATE_PREPARED); }
			virtual bool VIsLoaded(void) const { return (m_loadingState == LoadingState::LOADSTATE_LOADED); }
			virtual bool VIsLoading() const { return (m_loadingState == LoadingState::LOADSTATE_LOADING); }
			virtual void VSetToLoaded(void) { (m_loadingState = LoadingState::LOADSTATE_LOADED); }

			/** Returns the number of times this resource has changed state, which
				generally means the number of times it has been loaded. Objects that
				build derived data based on the resource can check this value against
				a copy they kept last time they built this derived data, in order to
				know whether it needs rebuilding. This is a nice way of monitoring
				changes without having a tightly-bound callback.
			*/
			virtual size_t getStateCount() const { return m_stateCount; }

			/** Manually mark the state of this resource as having been changed.
			@remarks
				You only need to call this from outside if you explicitly want derived
				objects to think this object has changed. @see getStateCount.
			*/
			virtual void _dirtyState();

			/** Calculate the size of a resource; this will only be called after 'load' */
			virtual size_t VCalculateSize(void) const;

		protected:
			Resource() : m_creator(0), m_handle(0), m_sizeInBytes(0), m_loadingState(LoadingState::LOADSTATE_UNLOADED) { }

			/** Internal hook to perform actions before the load process, but
				after the resource has been marked as 'loading'.
			@note Mutex will have already been acquired by the loading thread.
			*/
			virtual void VPreLoadImpl(void) {}

			/** Internal hook to perform actions after the load process, but
				before the resource has been marked as fully loaded.
			@note Mutex will have already been acquired by the loading thread.
			*/
			virtual void VPostLoadImpl(void) {}

			/** Internal hook to perform actions before the unload process.
			@note Mutex will have already been acquired by the unloading thread.
			*/
			virtual void VPreUnloadImpl(void) {}

			/** Internal hook to perform actions after the unload process, but
				before the resource has been marked as fully unloaded.
			@note Mutex will have already been acquired by the unloading thread.
			*/
			virtual void VPostUnloadImpl(void) {}

			/** Internal implementation of the meat of the 'prepare' action.
			*/
			virtual void VPrepareImpl(void) {}

			/** Internal function for undoing the 'prepare' action.  Called when
				the load is completed, and when resources are unloaded when they
			are prepared but not yet loaded.
			*/
			virtual void VUnprepareImpl(void) {}

			/** Internal implementation of the meat of the 'load' action, only called if this
				resource is not being loaded from a ManualResourceLoader.
			*/
			virtual void VLoadImpl(void) = 0;

			/** Internal implementation of the 'unload' action; called regardless of
				whether this resource is being loaded from a ManualResourceLoader.
			*/
			virtual void VUnloadImpl(void) = 0;

			ResourceManager* m_creator; /// Creator
			std::string m_name; /// Unique name of the resource
			ResourceHandle m_handle; /// Numeric handle for more efficient look up than name
			size_t m_sizeInBytes; /// The size of the resource in bytes
			LoadingState m_loadingState; /// Is the resource currently loaded? 
			size_t m_stateCount; /// State count, the number of times this resource has changed state
		};

	}
}