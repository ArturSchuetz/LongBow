#pragma once
#include "BowPrerequisites.h"

namespace bow {

	class Actor;
		typedef std::shared_ptr<Actor> ActorPtr;
		typedef std::map<unsigned int, ActorPtr> ActorMap;

	class ActorComponent;
		typedef std::shared_ptr<ActorComponent> ActorComponentPtr;
		typedef std::map<unsigned int, ActorComponentPtr> ActorComponentMap;

	class ActorFactory;
}
