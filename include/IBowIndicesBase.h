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
		public:
			IIndicesBase(IndicesType type) : m_type(type) {}
			virtual ~IIndicesBase() = 0;


			virtual IndicesType GetDatatype()
			{
				return m_type;
			}

		private:
			const IndicesType m_type;
		};

	}
}