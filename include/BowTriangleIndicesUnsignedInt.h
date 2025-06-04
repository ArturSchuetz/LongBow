/**
 * @file BowTriangleIndicesUnsignedInt.h
 * @brief Declarations for BowTriangleIndicesUnsignedInt.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"

#include <list>

namespace bow {

	struct TriangleIndicesUnsignedInt
	{
	public:
		TriangleIndicesUnsignedInt(unsigned int ui0, unsigned int ui1, unsigned int ui2) : m_index0(ui0), m_index1(ui1), m_index2(ui2) {}

		unsigned int GetIndex(unsigned int index)
		{
			switch (index)
			{
			case 0:
				return m_index0;
				break;
			case 1:
				return m_index1;
				break;
			case 2:
				return m_index2;
				break;
			default:
				return -1;
				break;
			}
		}

	private:
		unsigned int m_index0;
		unsigned int m_index1;
		unsigned int m_index2;
	};

}
