#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "BowResourcesPredeclares.h"

namespace bow {

	class ResourceManager
	{
	public:
		ResourceManager();
		virtual ~ResourceManager();

		/** Creates a new blank resource, but does not immediately load it.
		@remarks
			Resource managers handle disparate types of resources, so if you want
			to get at the detailed interface of this resource, you'll have to
			cast the result to the subclass you know you're creating.
		@param name The unique name of the resource
		*/
		ResourcePtr CreateResource(const std::string& name);

		/** Create a new resource, or retrieve an existing one with the same
			name if it already exists.
		@remarks
			This method performs the same task as calling getByName() followed
			by create() if that returns null. The advantage is that it does it
			in one call so there are no race conditions if using multiple
			threads that could cause getByName() to return null, but create() to
			fail because another thread created a resource in between.
		@see ResourceManager::createResource
		@see ResourceManager::getResourceByName
		*/
		ResourcePtr CreateOrRetrieve(const std::string& name);

		/** Gets the current memory usage, in bytes. */
		size_t getMemoryUsage() const { return m_memoryUsage; }

		/** Retrieves a pointer to a resource by name, or null if the resource does not exist.
		*/
		virtual ResourcePtr VGetResource(const std::string& name);

		/** Retrieves a pointer to a resource by handle, or null if the resource does not exist.
		*/
		virtual ResourcePtr VGetResource(ResourceHandle handle);

		/// Returns whether the named resource exists in this manager
		bool ResourceExists(const std::string& name) { return (bool)VGetResource(name); }

		/// Returns whether a resource with the given handle exists in this manager
		bool ResourceExists(ResourceHandle handle) { return (bool)VGetResource(handle); }

		/** Generic prepare method, used to create a Resource specific to this
			ResourceManager without using one of the specialised 'prepare' methods
			(containing per-Resource-type parameters).
		@param name The name of the Resource
		*/
		ResourcePtr Prepare(const std::string& name);

		/** Generic load method, used to create a Resource specific to this
			ResourceManager without using one of the specialised 'load' methods
			(containing per-Resource-type parameters).
		@param name The name of the Resource
		*/
		ResourcePtr Load(const std::string& name);

		/** Unloads a single resource by name.
		@remarks
			Unloaded resources are not removed, they simply free up their memory
			as much as they can and wait to be reloaded.
		@see ResourceGroupManager for unloading of resource groups.
		*/
		void Unload(const std::string& name);

		/** Unloads a single resource by handle.
		@remarks
			Unloaded resources are not removed, they simply free up their memory
			as much as they can and wait to be reloaded.
		@see ResourceGroupManager for unloading of resource groups.
		*/
		void Unload(ResourceHandle handle);

		/** Unloads all resources.
		@remarks
			Unloaded resources are not removed, they simply free up their memory
			as much as they can and wait to be reloaded.
		*/
		virtual void VUnloadAll();

		/** Remove a single resource.
		@remarks
			Removes a single resource, meaning it will be removed from the list
			of valid resources in this manager, also causing it to be unloaded.
		@note
			The word 'Destroy' is not used here, since
			if any other pointers are referring to this resource, it will persist
			until they have finished with it; however to all intents and purposes
			it no longer exists and will likely get destroyed imminently.
		@note
			If you do have shared pointers to resources hanging around after the
			ResourceManager is destroyed, you may get problems on destruction of
			these resources if they were relying on the manager (especially if
			it is a plugin). If you find you get problems on shutdown in the
			destruction of resources, try making sure you release all your
			shared pointers before you shutdown OGRE.
		*/
		void Remove(const ResourcePtr& resource);

		/// @overload
		void Remove(const std::string& name);

		/// @overload
		void Remove(ResourceHandle handle);

		/** Removes all resources.
		@note
			The word 'Destroy' is not used here, since
			if any other pointers are referring to these resources, they will persist
			until they have been finished with; however to all intents and purposes
			the resources no longer exist and will get destroyed imminently.
		@note
			If you do have shared pointers to resources hanging around after the
			ResourceManager is destroyed, you may get problems on destruction of
			these resources if they were relying on the manager (especially if
			it is a plugin). If you find you get problems on shutdown in the
			destruction of resources, try making sure you release all your
			shared pointers before you shutdown OGRE.
		*/
		virtual void VRemoveAll();

	protected:
		/** Allocates the next handle. */
		ResourceHandle GetNextHandle();

		/** Create a new resource instance compatible with this manager (no custom
			parameters are populated at this point).
		@remarks
			Subclasses must override this method and create a subclass of Resource.
		@param name The unique name of the resource
		*/
		virtual Resource* VCreateImpl(const std::string& name, ResourceHandle handle) = 0;

		/** Add a newly created resource to the manager (note weak reference) */
		virtual void VAddImpl(ResourcePtr& resource);

		/** Remove a resource from this manager; remove it from the lists. */
		virtual void VRemoveImpl(const ResourcePtr& resource);

		std::map<std::string, ResourcePtr> m_resourcesByName;
		std::map<ResourceHandle, ResourcePtr> m_resourcesByHandle;

		ResourceHandle m_nextHandle;
		size_t m_memoryUsage;

		/// String identifying the resource type this manager handles
		std::string m_resourceType;

	private:
		ResourceManager(const ResourceManager&){} // You shall not copy
		ResourceManager& operator = (const ResourceManager&) { return *this; }	
	};
}