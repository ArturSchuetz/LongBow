#include "BowActorManager.h"

#include "BowActor.h"

namespace bow
{
	static std::shared_ptr<ActorManager> Instance;

	ActorManager::ActorManager(void)
	{

	}

	ActorManager::~ActorManager(void)
	{

	}

	ActorManager& ActorManager::GetInstance(void)
	{
		if (Instance.get() == nullptr)
		{
			Instance = std::shared_ptr<ActorManager>(new ActorManager());
			Instance->Initialize();
		}
		return *Instance.get();
	}

	void ActorManager::Initialize(void)
	{

	}
}
