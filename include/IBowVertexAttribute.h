#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Core {

		enum class VertexAttributeType : char
		{
			UnsignedByte,
			Float,
			FloatVector2,
			FloatVector3,
			FloatVector4,
		};

		struct IVertexAttribute
		{
		protected:
			IVertexAttribute(const std::string& name, VertexAttributeType type) : Name(name), Type(type)
			{
			}

			virtual ~IVertexAttribute() = 0;

		public:
			const std::string			Name;
			const VertexAttributeType	Type;
		};

		template <class T>
		struct VertexAttribute : IVertexAttribute
		{
		protected:
			VertexAttribute(const std::string& name, VertexAttributeType type) : IVertexAttribute(name, type), Values(std::vector<T>())
			{
			}

			VertexAttribute(const std::string& name, VertexAttributeType type, int capacity) : IVertexAttribute(name, type), Values(std::vector<T>(capacity))
			{
			}

		public:
			const std::vector<T> Values;
		};

	}
}