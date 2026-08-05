/**
 * @file BowActorComponentManager.h
 * @brief Declarations for BowActorComponentManager.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <CoreSystems/BowCorePredeclares.h>
#include <GameFoundation/BowGameFoundationPredeclares.h>

#include <CoreSystems/BowMath.h>

namespace bow
{
	class ActorComponentManager
	{
		friend ActorComponent;

	public:
		virtual ~ActorComponentManager(void);

		static ActorComponentManager& GetInstance(void);

	private:
		ActorComponentManager(void);
		ActorComponentManager(const ActorComponentManager&) {} // You shall not copy
		ActorComponentManager& operator = (const ActorComponentManager&) { return *this; }

		void Initialize();
		
		void AddComponent(ActorComponent* component);
		void RemoveComponent(ActorComponent* component);

		unsigned int GetNextComponentId(void) { ++m_lastComponentId; return m_lastComponentId; }

		unsigned int m_lastComponentId;
		std::map<unsigned int, ActorComponent*> m_components;
	};
}
