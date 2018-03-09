#include "BowActorComponent.h"

#include "BowMeshRendererComponent.h"

namespace bow
{
	MeshRendererComponent::MeshRendererComponent(MeshPtr mesh, MaterialPtr material) : ActorComponent()
	{
		m_meshAttribute = mesh->CreateAttribute("in_Position", "in_Normal", "in_TexCoord");
		m_material = material;
	}

	MeshRendererComponent::MeshRendererComponent(MeshAttribute meshAttribute, MaterialPtr material) : ActorComponent()
	{
		m_meshAttribute = meshAttribute;
		m_material = material;
	}

	MeshRendererComponent::~MeshRendererComponent(void)
	{

	}
}
