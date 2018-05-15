#include "BowD3D12TextureUnit.h"
#include "BowLogger.h"

#include "BowD3D12Texture2D.h"
#include "BowD3D12TextureSampler.h"

namespace bow {

	D3DTextureUnit::D3DTextureUnit(D3DRenderDevice* device, unsigned int index, ICleanableObserver* observer) : 
		m_device(device), 
		m_DirtyFlags(DirtyFlags::None),
		m_textureUnitIndex(index), 
		m_observer(observer),
		m_textureUnit(index)
	{
		m_texture = D3DTexture2DPtr(nullptr);
		m_textureSampler = D3DTextureSamplerPtr(nullptr);
	}

	D3DTexture2DPtr D3DTextureUnit::GetTexture()
	{
		return m_texture;
	}

	void D3DTextureUnit::SetTexture(D3DTexture2DPtr texture)
	{
		if (m_texture != texture)
		{
			if (m_DirtyFlags == DirtyFlags::None)
			{
				m_observer->NotifyDirty(this);
			}

			m_DirtyFlags = (DirtyFlags)(m_DirtyFlags | DirtyFlags::Texture);
			m_texture = texture;
		}
	}

	D3DTextureSamplerPtr D3DTextureUnit::GetSampler()
	{
		return m_textureSampler;
	}

	void D3DTextureUnit::SetSampler(D3DTextureSamplerPtr sampler)
	{
		if (m_textureSampler != sampler)
		{
			if (m_DirtyFlags == DirtyFlags::None)
			{
				m_observer->NotifyDirty(this);
			}
			m_DirtyFlags = (DirtyFlags)(m_DirtyFlags | DirtyFlags::TextureSampler);
			m_textureSampler = sampler;
		}
	}

	void D3DTextureUnit::CleanLastTextureUnit()
	{
		//
		// If the last texture unit has a texture attached, it
		// is cleaned even if it isn't dirty because the last
		// texture unit is used for texture uploads and downloads in
		// D3DTexture2D, the texture unit could be dirty without
		// knowing it.
		//
		if (m_texture != nullptr)
		{
			m_DirtyFlags = (DirtyFlags)(m_DirtyFlags | DirtyFlags::Texture);
		}

		Clean();
	}

	void D3DTextureUnit::Clean()
	{
		if (m_DirtyFlags != DirtyFlags::None)
		{
			Validate();

			/*
			glActiveTexture(m_textureUnit);

			if ((m_DirtyFlags & DirtyFlags::Texture) == DirtyFlags::Texture)
			{
				if (m_texture != nullptr)
				{
					m_texture->Bind();
				}
				else
				{
					D3DTexture2D::UnBind(GL_TEXTURE_2D);
					D3DTexture2D::UnBind(GL_TEXTURE_RECTANGLE);
				}
			}

			if ((m_DirtyFlags & DirtyFlags::TextureSampler) == DirtyFlags::TextureSampler)
			{
				if (m_textureSampler != nullptr)
				{
					m_textureSampler->Bind(m_textureUnitIndex);
				}
				else
				{
					D3DTextureSampler::UnBind(m_textureUnitIndex);
				}
			}
			*/

			m_DirtyFlags = DirtyFlags::None;
		}
	}

	void D3DTextureUnit::Validate()
	{
		if (m_texture != nullptr)
		{
			if (m_textureSampler == nullptr)
			{
				LOG_FATAL("A texture sampler must be assigned to a texture unit with one or more bound textures.");
			}

			/*
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
			*/
		}
	}

}
