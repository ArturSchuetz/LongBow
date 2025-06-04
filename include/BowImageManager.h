/**
 * @file BowImageManager.h
 * @brief Declarations for BowImageManager.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowResourcesPredeclares.h"

#include "BowResourceManager.h"
#include "BowImage.h"

namespace bow {

	class ImageManager : public ResourceManager
	{
	public:
		~ImageManager();

		static ImageManager& GetInstance();

		void Initialize(void);

		/// Create a new mesh
		/// @see ResourceManager::createResource
		ImagePtr Create(const std::string& name);

		ResourcePtr CreateOrRetrieve(const std::string& name);

		ImagePtr Prepare(const std::string& filePath);

		ImagePtr Load(const std::string& filePath);

		ImagePtr CreateManual(const std::string& name);

	private:
		ImageManager(const ImageManager&) {}; // You shall not copy
		ImageManager& operator=(const ImageManager&) { return *this; }
		ImageManager(void);

		/// @copydoc ResourceManager::createImpl
		virtual Resource* VCreateImpl(const std::string& name, ResourceHandle handle);
	};

}
