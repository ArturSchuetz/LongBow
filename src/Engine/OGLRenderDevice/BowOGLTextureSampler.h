#pragma once
#include "BowPrerequisites.h"

#include "IBowTextureSampler.h"

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLSamplerName> OGLSamplerNamePtr;

		class OGLTextureSampler : public ITextureSampler
		{
		public:
			OGLTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy = 1);

			void Bind(int textureUnitIndex);
			static void UnBind(int textureUnitIndex);

		private:
			const OGLSamplerNamePtr m_name;
		};

		typedef std::shared_ptr<OGLTextureSampler> OGLTextureSamplerPtr;

	}
}