/**
 * @file IBowIndicesBase.h
 * @brief Declarations for IBowIndicesBase.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace bow {

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

		virtual unsigned int Size() = 0;

	public:
		const IndicesType Type;
	};

	typedef std::shared_ptr<IIndicesBase> IndicesBasePtr;

}
