#pragma once
#include "LongBow.h"
#include "IBowTextureSampler.h"

#include "OGLSamplerName.h"

#include <GL\glew.h>

namespace Bow {

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