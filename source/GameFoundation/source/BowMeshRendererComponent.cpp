#include <GameFoundation/BowActorComponent.h>

#include <GameFoundation/BowMeshRendererComponent.h>

namespace bow
{
	MeshRendererComponent::MeshRendererComponent(MeshPtr mesh, MaterialCollectionPtr material) : ActorComponent()
	{
		m_meshAttribute = mesh->CreateAttribute("in_Position", "in_Normal", "in_TexCoord");
		m_material = material;
	}

	MeshRendererComponent::MeshRendererComponent(MeshAttribute meshAttribute, MaterialCollectionPtr material) : ActorComponent()
	{
		m_meshAttribute = meshAttribute;
		m_material = material;
	}

	MeshRendererComponent::~MeshRendererComponent(void)
	{

	}
}
