#pragma once
#include "BowPrerequisites.h"
#include "BowGameFoundationPredeclares.h"

namespace bow
{
	class ActorComponent
	{
	public:
		explicit ActorComponent(void);
		virtual ~ActorComponent(void);

		unsigned int GetId();
	private:
		ActorComponent(const ActorComponent&) : m_id(0) {} // You shall not copy
		ActorComponent& operator = (const ActorComponent&) { return *this; }

		const unsigned int m_id; // unique id for the component
	};
}
