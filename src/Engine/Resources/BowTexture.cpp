#include "BowResources.h"
#include "BowTexture.h"

namespace Bow {
	namespace Core {

		Texture::Texture(ResourceManager* creator, const std::string& name, ResourceHandle handle)
			: Resource(creator, name, handle)
		{
			LOG_TRACE("Creating Texture: %s", name);
		}

		Texture::~Texture()
		{        
			// have to call this here reather than in Resource destructor
			// since calling virtual methods in base destructors causes crash
			VUnload();
		}

		// ============================================================

		void Texture::VPrepareImpl(void)
		{
			// fully prebuffer into host RAM
		}

		void Texture::VUnprepareImpl(void)
		{

		}

		void Texture::VLoadImpl(void)
		{

		}

		void Texture::VPostLoadImpl(void)
		{

		}

		void Texture::VUnloadImpl(void)
		{

		}

		size_t Texture::VCalculateSize(void) const
		{
			// calculate GPU size
			size_t ret = 0;
			

			return ret;
		}

	}
}