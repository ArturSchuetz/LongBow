#include "BowD3D12TextureUnits.h"
#include "BowLogger.h"

#include "BowD3D12TextureUnit.h"
#include "BowD3D12Texture2D.h"

#include <algorithm>

namespace bow {

	D3DTextureUnits::D3DTextureUnits(D3DRenderDevice* device) : 
		m_device(device), 
		m_numberOfTextureUnits(0)
	{
		// Device.NumberOfTextureUnits is not initialized yet.
		//		
		m_numberOfTextureUnits = 31;

		for (int i = 0; i < m_numberOfTextureUnits; ++i)
		{
			m_textureUnits[i] = D3DTextureUnitPtr(new D3DTextureUnit(device, i, this));
		}
		m_dirtyTextureUnits = std::list<ICleanable*>();
		m_lastTextureUnit = m_textureUnits[m_numberOfTextureUnits - 1];
	}


	void D3DTextureUnits::Clean()
	{
		for (auto it = m_dirtyTextureUnits.begin(); it != m_dirtyTextureUnits.end(); it++)
		{
			(*it)->Clean();
		}
		m_dirtyTextureUnits.clear();
		m_lastTextureUnit->CleanLastTextureUnit();
	}


	void D3DTextureUnits::NotifyDirty(ICleanable* value)
	{
		m_dirtyTextureUnits.push_back(value);
	}


	int D3DTextureUnits::GetMaxTextureUnits()
	{
		return m_numberOfTextureUnits;
	}


	void D3DTextureUnits::SetTexture(int index, D3DTexture2DPtr texture)
	{
		m_textureUnits[index]->SetTexture(texture);
	}


	void D3DTextureUnits::SetSampler(int index, D3DTextureSamplerPtr sampler)
	{
		m_textureUnits[index]->SetSampler(sampler);
	}

}
