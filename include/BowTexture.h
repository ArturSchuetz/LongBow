/**
 * @file BowTexture.h
 * @brief Declarations for BowTexture.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {
	namespace Core {

		// ---------------------------------------------------------------------------
		/** @brief A mesh represents geometry without any material.
		*/
		class Texture : public Resource
		{
		public:
			Texture(ResourceManager* creator, const std::string& name, ResourceHandle handle);
			~Texture();

		private:
			/** Loads the mesh from disk.  This call only performs IO, it
				does not parse the bytestream or check for any errors therein.
				It also does not set up submeshes, etc.  You have to call load()
				to do that.
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

			/// @copydoc Resource::VCalculateSize
			size_t VCalculateSize(void) const;
		};
	}
}