#include "BowActor.h"

#include "BowActorManager.h"

namespace bow
{
	Actor::Actor() : m_id(ActorManager::GetInstance().GetNextActorId())
	{
	}

	Actor::~Actor(void)
	{

	}

	void Actor::Activate()
	{
		m_isActive = true;
	}

	void Actor::Deactivate()
	{
		m_isActive = false;
	}

	bool Actor::IsActive() const
	{
		return m_isActive;
	}

	const char* Actor::GetName() const
	{
		return m_name.c_str();
	}

	void Actor::SetName(const char* name)
	{
		m_name = name;
	}

	void Actor::SetParent(Actor* parent)
	{
		if (m_parent != nullptr)
		{
			m_parent->RemoveChild(this);
			m_parent = nullptr;
		}

		if (parent != nullptr)
		{
			parent->AddChild(this);
			parent = nullptr;
		}
	}

	const Transform<float>& Actor::GetTransform() const
	{
		return m_transform;
	}

	void Actor::SetTransform(const Transform<float>& transform)
	{
		m_transform = transform;
	}

	bool Actor::AddComponent(ActorComponent* pComponent)
	{
		return false;
	}

	bool Actor::RemoveComponent(ActorComponent* pComponent)
	{
		return false;
	}

	std::vector<ActorComponent*> Actor::GetComponents()
	{
		return std::vector<ActorComponent*>();
	}

	void Actor::AddChild(Actor* parent)
	{

	}

	void Actor::RemoveChild(Actor* parent)
	{

	}

	unsigned int Actor::GetChildCount() const
	{
		return 0;
	}

}
