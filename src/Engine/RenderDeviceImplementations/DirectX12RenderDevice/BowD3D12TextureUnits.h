#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"
#include "BowD3D12RenderDevicePredeclares.h"

#include "IBowCleanableObserver.h"

namespace bow {

	class D3DTextureUnits : ICleanableObserver
	{
	public:
		D3DTextureUnits(D3DRenderDevice* device);

		void Clean();
		void NotifyDirty(ICleanable* value);

		int		GetMaxTextureUnits();
		void	SetTexture(int index, D3DTexture2DPtr texture);
		void	SetSampler(int index, D3DTextureSamplerPtr sampler);

	private:
		D3DRenderDevice* m_device;

		std::unordered_map<unsigned int, D3DTextureUnitPtr>	m_textureUnits;
		std::list<ICleanable*>								m_dirtyTextureUnits;
		D3DTextureUnitPtr									m_lastTextureUnit;
		int													m_numberOfTextureUnits;
	};

	typedef std::shared_ptr<D3DTextureUnits> D3DTextureUnitsPtr;

}
