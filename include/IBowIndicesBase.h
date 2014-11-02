#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Core {

		enum class IndicesType : char
		{
			UnsignedShort,
			UnsignedInt
		};

		class IIndicesBase
		{
		protected:
			IIndicesBase(IndicesType type) : Type(type)
			{
			}

		public:
			virtual ~IIndicesBase(){}

		public:
			const IndicesType Type;
		};

	}
}