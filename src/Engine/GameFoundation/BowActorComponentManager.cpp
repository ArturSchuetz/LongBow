#include "BowActorComponentManager.h"

#include "BowActorComponent.h"

namespace bow
{
	static std::shared_ptr<ActorComponentManager> Instance;

	ActorComponentManager::ActorComponentManager(void)
	{

	}

	ActorComponentManager::~ActorComponentManager(void)
	{

	}

	ActorComponentManager& ActorComponentManager::GetInstance(void)
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<ActorComponentManager>(new ActorComponentManager());
			Instance->Initialize();
		}
		return *Instance.get();
	}

	void ActorComponentManager::Initialize(void)
	{

	}

	void ActorComponentManager::AddComponent(ActorComponent* component)
	{
		if (m_components.at(component->GetId()) == nullptr)
		{
			m_components.insert(std::pair<unsigned int, ActorComponent*>(component->GetId(), component));
		}
		else
		{
			LOG_ERROR("Component already exists");
		}
	}

	void ActorComponentManager::RemoveComponent(ActorComponent* component)
	{
		if (m_components.at(component->GetId()) != nullptr)
		{
			m_components.erase(component->GetId());
		}
		else
		{
			LOG_ERROR("Component does not exist in list");
		}
	}

}
