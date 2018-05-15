#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowCleanableObserver.h"

namespace bow {

	typedef std::shared_ptr<class D3DTexture2D> D3DTexture2DPtr;
	typedef std::shared_ptr<class D3DTextureSampler> D3DTextureSamplerPtr;
	typedef unsigned int GLenum;

	class D3DTextureUnit : ICleanable
	{
	public:
		D3DTextureUnit(D3DRenderDevice* device, unsigned int index, ICleanableObserver* observer);

		D3DTexture2DPtr GetTexture();
		void SetTexture(D3DTexture2DPtr texture);

		D3DTextureSamplerPtr GetSampler();
		void SetSampler(D3DTextureSamplerPtr sampler);

		void CleanLastTextureUnit();
		void Clean();
	private:
		D3DRenderDevice* m_device;

		void Validate();

		typedef enum TYPE_DirtyFlags
		{
			None = 0,
			Texture = 1,
			TextureSampler = 2,
			All = Texture | TextureSampler
		} DirtyFlags;

		const int					m_textureUnitIndex;
		const GLenum				m_textureUnit;
		ICleanableObserver* const	m_observer;

		D3DTexture2DPtr			m_texture;
		D3DTextureSamplerPtr	m_textureSampler;
		DirtyFlags				m_DirtyFlags;
	};

	typedef std::shared_ptr<D3DTextureUnit> D3DTextureUnitPtr;

}
