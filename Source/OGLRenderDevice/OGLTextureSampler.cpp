#include "OGLTextureSampler.h"
#include "OGLTypeConverter.h"

#include <GL/glew.h>

namespace Bow
{
	
	OGLTextureSampler::OGLTextureSampler(TextureMinificationFilter minificationFilter, TextureMagnificationFilter magnificationFilter, TextureWrap wrapS, TextureWrap wrapT, float maximumAnistropy)
		: ITextureSampler(minificationFilter, magnificationFilter, wrapS, wrapT, maximumAnistropy), m_name(new OGLSamplerName())
	{
		FN("OGLTextureSampler::OGLTextureSampler()");
		
        int glMinificationFilter = (int)OGLTypeConverter::To(minificationFilter);
        int glMagnificationFilter = (int)OGLTypeConverter::To(magnificationFilter);
        int glWrapS = (int)OGLTypeConverter::To(wrapS);
        int glWrapT = (int)OGLTypeConverter::To(wrapT);

		glSamplerParameteri(m_name->GetValue(), GL_TEXTURE_MIN_FILTER, glMinificationFilter);
        glSamplerParameteri(m_name->GetValue(), GL_TEXTURE_MAG_FILTER, glMagnificationFilter);
		glSamplerParameteri(m_name->GetValue(), GL_TEXTURE_WRAP_S, glWrapS);
        glSamplerParameteri(m_name->GetValue(), GL_TEXTURE_WRAP_T, glWrapT);
		
		/*
        if (Device.Extensions.AnisotropicFiltering)
        {
			glSamplerParameteri(m_name->GetValue(), GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistropy);
        }
        else
        {
            LOG_ASSERT(!(maximumAnistropy != 1), "Anisotropic filtering is not supported.  The extension GL_EXT_texture_filter_anisotropic was not found.");
        }
		*/
	}

	void OGLTextureSampler::Bind(int textureUnitIndex)
	{
		FN("OGLTextureSampler::Bind()");
		glBindSampler(textureUnitIndex, m_name->GetValue());
	}

	void OGLTextureSampler::UnBind(int textureUnitIndex)
	{
		FN("OGLTextureSampler::UnBind()");
		glBindSampler(textureUnitIndex, 0);
	}


}