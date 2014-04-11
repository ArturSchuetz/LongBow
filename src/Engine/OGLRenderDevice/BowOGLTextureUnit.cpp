#include "BowOGLTextureUnit.h"
#include "BowLogger.h"

#include "BowOGLTexture2D.h"
#include "BowOGLTextureSampler.h"

#include <GL/glew.h>

namespace Bow {
	namespace Renderer {


		OGLTextureUnit::OGLTextureUnit(unsigned int index, ICleanableObserver* observer) : m_dirtyFlags(DirtyFlags::None), m_textureUnitIndex(index), m_observer(observer), m_textureUnit(GL_TEXTURE0 + index)
		{
			m_texture = OGLTexture2DPtr(nullptr);
			m_textureSampler = OGLTextureSamplerPtr(nullptr);
		}

		OGLTexture2DPtr OGLTextureUnit::GetTexture()
		{
			return m_texture;
		}

		void OGLTextureUnit::SetTexture(OGLTexture2DPtr texture)
		{
			if (m_texture != texture)
			{
				if (m_dirtyFlags == DirtyFlags::None)
				{
					m_observer->NotifyDirty(this);
				}

				m_dirtyFlags = (DirtyFlags)(m_dirtyFlags | DirtyFlags::Texture);
				m_texture = texture;
			}
		}

		OGLTextureSamplerPtr OGLTextureUnit::GetSampler()
		{
			return m_textureSampler;
		}

		void OGLTextureUnit::SetSampler(OGLTextureSamplerPtr sampler)
		{
			if (m_textureSampler != sampler)
			{
				if (m_dirtyFlags == DirtyFlags::None)
				{
					m_observer->NotifyDirty(this);
				}
				m_dirtyFlags = (DirtyFlags)(m_dirtyFlags | DirtyFlags::TextureSampler);
				m_textureSampler = sampler;
			}
		}

		void OGLTextureUnit::CleanLastTextureUnit()
		{
			//
			// If the last texture unit has a texture attached, it
			// is cleaned even if it isn't dirty because the last
			// texture unit is used for texture uploads and downloads in
			// OGLTexture2D, the texture unit could be dirty without
			// knowing it.
			//
			if (m_texture != nullptr)
			{
				m_dirtyFlags = (DirtyFlags)(m_dirtyFlags | DirtyFlags::Texture);
			}

			Clean();
		}

		void OGLTextureUnit::Clean()
		{
			if (m_dirtyFlags != DirtyFlags::None)
			{
				Validate();

				glActiveTexture(m_textureUnit);

				if ((m_dirtyFlags & DirtyFlags::Texture) == DirtyFlags::Texture)
				{
					if (m_texture != nullptr)
					{
						m_texture->Bind();
					}
					else
					{
						OGLTexture2D::UnBind(GL_TEXTURE_2D);
						OGLTexture2D::UnBind(GL_TEXTURE_RECTANGLE);
					}
				}

				if ((m_dirtyFlags & DirtyFlags::TextureSampler) == DirtyFlags::TextureSampler)
				{
					if (m_textureSampler != nullptr)
					{
						m_textureSampler->Bind(m_textureUnitIndex);
					}
					else
					{
						OGLTextureSampler::UnBind(m_textureUnitIndex);
					}
				}

				m_dirtyFlags = DirtyFlags::None;
			}
		}

		void OGLTextureUnit::Validate()
		{
			if (m_texture != nullptr)
			{
				if (m_textureSampler == nullptr)
				{
					LOG_FATAL("A texture sampler must be assigned to a texture unit with one or more bound textures.");
				}

				if (m_texture->GetTarget() == GL_TEXTURE_RECTANGLE)
				{
					if (m_textureSampler->MinificationFilter != TextureMinificationFilter::Linear && m_textureSampler->MinificationFilter != TextureMinificationFilter::Nearest)
					{
						LOG_FATAL("The texture sampler is incompatible with the rectangle texture bound to the same texture unit.  Rectangle textures only support linear and nearest minification filters.");
					}

					if (m_textureSampler->WrapS == TextureWrap::Repeat || m_textureSampler->WrapS == TextureWrap::MirroredRepeat || m_textureSampler->WrapT == TextureWrap::Repeat || m_textureSampler->WrapT == TextureWrap::MirroredRepeat)
					{
						LOG_FATAL("The texture sampler is incompatible with the rectangle texture bound to the same texture unit.  Rectangle textures do not support repeat or mirrored repeat wrap modes.");
					}
				}
			}
		}

	}
}