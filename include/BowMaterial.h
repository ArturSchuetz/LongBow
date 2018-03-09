#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A image represents a bitmap loaded from any file format.
	*/
	class Material : public Resource
	{
	public:
		Material(ResourceManager* creator, const std::string& name, ResourceHandle handle);
		~Material();

	private:
		/** Loads the image from disk.  This call only performs IO, it
			does not parse the bytestream or check for any errors therein.
			You have to call load() to do that.
		*/
		void VPrepareImpl(void);

		/** Destroys data cached by prepareImpl.
		*/
		void VUnprepareImpl(void);

		/// @copydoc Resource::VLoadImpl
		void VLoadImpl(void);

		/// @copydoc Resource::VPostLoadImpl
		void VPostLoadImpl(void);

		/// @copydoc Resource::VUnloadImpl
		void VUnloadImpl(void);

		char* m_dataFromDisk;
	};
}