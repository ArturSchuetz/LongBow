/**
 * @file BowActorManager.h
 * @brief Declarations for BowActorManager.
 */

#pragma once
#include <CoreSystems/BowCorePredeclares.h>
#include <GameFoundation/BowGameFoundationPredeclares.h>

#include <CoreSystems/BowMath.h>

namespace bow
{
	class ActorManager
	{
		friend Actor;

	public:
		virtual ~ActorManager(void);

		static ActorManager& GetInstance(void);

	private:
		ActorManager(void);
		ActorManager(const ActorManager&) {} // You shall not copy
		ActorManager& operator = (const ActorManager&) { return *this; }

		void Initialize();

		unsigned int GetNextActorId(void) { ++m_lastActorId; return m_lastActorId; }

		unsigned int m_lastActorId;
	};
}
