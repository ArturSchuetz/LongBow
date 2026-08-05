/**
 * @file BowMeshRendererComponent.h
 * @brief Declarations for BowMeshRendererComponent.
 */

#pragma once
#include <CoreSystems/BowCorePredeclares.h>
#include <GameFoundation/BowGameFoundationPredeclares.h>

#include <Resources/Resources/BowMesh.h>
#include <Resources/Resources/BowMaterial.h>

#include <CoreSystems/Geometry/BowMeshAttribute.h>

#include <RenderDevice/BowRenderState.h>

namespace bow
{
	class MeshRendererComponent : public ActorComponent
	{
	public:
		explicit MeshRendererComponent(MeshPtr mesh, MaterialCollectionPtr material);
		explicit MeshRendererComponent(MeshAttribute mesh, MaterialCollectionPtr material);
		~MeshRendererComponent(void);

	private:
		MeshRendererComponent(const MeshRendererComponent&) : ActorComponent() {} // You shall not copy
		MeshRendererComponent& operator = (const MeshRendererComponent&) { return *this; }

		MeshAttribute m_meshAttribute;
		RenderState m_renderState;

		MaterialCollectionPtr m_material;
	};
}
