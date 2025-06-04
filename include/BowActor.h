/**
 * @file BowActor.h
 * @brief Declarations for BowActor.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowGameFoundationPredeclares.h"

#include "BowTransform.h"
#include "BowActorComponent.h"

namespace bow
{
	class Actor
	{
	public:
		explicit Actor(void);
		~Actor(void);

		void Activate(void);
		void Deactivate(void);

		bool IsActive(void) const;

		const char* GetName(void) const;
		void SetName(const char* name);

		void SetParent(Actor* parent);

		const Transform<float>& GetTransform(void) const;
		void SetTransform(const Transform<float>& transform);

		bool AddComponent(ActorComponent* pComponent);
		bool RemoveComponent(ActorComponent* pComponent);

		std::vector<ActorComponent*> GetComponents(void);

	private:
		Actor(const Actor&) : m_id(0) {} // You shall not copy
		Actor& operator = (const Actor&) { return *this; }

		void AddChild(Actor* child);
		void RemoveChild(Actor* child);  
		unsigned int GetChildCount() const;

		const unsigned int	m_id; // unique id for the actor

		Transform<float>	m_transform;
		Actor*				m_parent;
		std::string			m_name;
		bool				m_isActive; 

		ActorComponentMap	m_components;
	};
}
