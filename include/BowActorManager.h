#pragma once
#include "BowPrerequisites.h"
#include "BowGameFoundationPredeclares.h"

#include "BowMath.h"

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
