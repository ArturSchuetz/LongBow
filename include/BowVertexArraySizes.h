#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

#include "BowLogger.h"

#include "BowIndexBufferDatatype.h"
#include "BowComponentDatatype.h"

namespace bow {

	class VertexArraySizes
	{
	public:
		static int SizeOf(IndexBufferDatatype type)
		{
			switch (type)
			{
			case IndexBufferDatatype::UnsignedShort:
				return sizeof(unsigned short);
			case IndexBufferDatatype::UnsignedInt:
				return sizeof(unsigned int);
			}

			LOG_FATAL("IndexBufferDatatype does not exist.");
			return -1;
		}

		static int SizeOf(ComponentDatatype type)
		{
			switch (type)
			{
			case ComponentDatatype::Byte:
			case ComponentDatatype::UnsignedByte:
				return sizeof(char);
			case ComponentDatatype::Short:
				return sizeof(short);
			case ComponentDatatype::UnsignedShort:
				return sizeof(unsigned short);
			case ComponentDatatype::Int:
				return sizeof(int);
			case ComponentDatatype::UnsignedInt:
				return sizeof(unsigned int);
			case ComponentDatatype::Float:
				return sizeof(float);
			}

			LOG_FATAL("ComponentDatatype does not exist.");
			return -1;
		}
	};

}
