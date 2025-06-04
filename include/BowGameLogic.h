/**
 * @file BowGameLogic.h
 * @brief Declarations for BowGameLogic.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowGameFoundationPredeclares.h"

#include "BowMath.h"

namespace bow
{
	class GameApplication
	{
	public:
		virtual ActorPtr VGetActor(const unsigned int actorId) = 0;
		virtual ActorPtr VCreateActor(const Matrix4x4<float> *initialTransform = NULL, const unsigned int serversActorId = 0) = 0;
		virtual void VDestroyActor(const unsigned int actorId) = 0;
		virtual bool VLoadGame(const char* levelResource) = 0;
		virtual void VSetProxy() = 0;
		virtual void VOnUpdate(float time, float elapsedTime) = 0;
		virtual void VChangeState(enum BaseGameState newState) = 0;
		virtual void VMoveActor(const unsigned int actorId, Matrix4x4<float> const &mat) = 0;

	protected:
		ActorMap		m_actors;

		unsigned int	m_LastActorId;

		ActorFactory	*m_pActorFactory;
	};
}
