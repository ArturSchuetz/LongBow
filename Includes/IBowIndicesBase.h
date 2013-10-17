#pragma once
#include "LongBow.h"

namespace Bow {

	enum class IndicesType : char
    {
        UnsignedShort,
        UnsignedInt
    };

	struct IIndicesBase
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