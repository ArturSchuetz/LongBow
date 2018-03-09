#include "BowActorComponent.h"

#include "BowActorComponentManager.h"

namespace bow
{
	ActorComponent::ActorComponent() : m_id(ActorComponentManager::GetInstance().GetNextComponentId())
	{
		ActorComponentManager::GetInstance().AddComponent(this);
	}

	ActorComponent::~ActorComponent(void)
	{
		ActorComponentManager::GetInstance().RemoveComponent(this);
	}

	unsigned int ActorComponent::GetId()
	{
		return m_id;
	}
}
