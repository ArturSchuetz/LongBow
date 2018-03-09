#pragma once
#include "BowPrerequisites.h"
#include "BowGameFoundationPredeclares.h"

#include "BowMesh.h"
#include "BowMaterial.h"

#include "BowMeshAttribute.h"

#include "BowRenderState.h"

namespace bow
{
	class MeshRendererComponent : public ActorComponent
	{
	public:
		explicit MeshRendererComponent(MeshPtr mesh, MaterialPtr material);
		explicit MeshRendererComponent(MeshAttribute mesh, MaterialPtr material);
		~MeshRendererComponent(void);

	private:
		MeshRendererComponent(const MeshRendererComponent&) : ActorComponent() {} // You shall not copy
		MeshRendererComponent& operator = (const MeshRendererComponent&) { return *this; }

		MeshAttribute m_meshAttribute;
		RenderState m_renderState;

		MaterialPtr m_material;
	};
}
