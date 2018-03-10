#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A image represents a bitmap loaded from any file format.
	*/
	class Image : public Resource
	{
		friend class ImageLoader_bmp;
		friend class ImageLoader_png;

	public:
		Image(ResourceManager* creator, const std::string& name, ResourceHandle handle);
		~Image();

		unsigned int GetSizeInBytes();
		unsigned int GetHeight();
		unsigned int GetWidth();
		unsigned char* GetData();

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

		int	m_width;
		int	m_height;
		int	m_sizeInBytes; 
		short m_numBitsPerPixel;
		std::vector<unsigned char> m_data;
	};
}