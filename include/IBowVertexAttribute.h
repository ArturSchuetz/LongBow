#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Core {

		/*enum class VertexAttributeType : char
		{
			UnsignedByte,
			Float,
			FloatVector2,
			FloatVector3,
			FloatVector4,
		};

		struct IVertexAttribute
		{
		public:
			IVertexAttribute(std::string name, VertexAttributeType type) : m_name(name), m_type(type) {}
			virtual ~IVertexAttribute() = 0;

			virtual std::string GetName()
			{
				return m_name;
			}

			virtual VertexAttributeType GetDatatype()
			{
				return m_type;
			}

		private:
			const std::string			m_name;
			const VertexAttributeType	m_type;
		};

		template <class T>
		struct VertexAttribute : IVertexAttribute
		{
		protected:
			VertexAttribute(std::string name, VertexAttributeType type) : IVertexAttribute(name, type), Values(std::list<T>())
			{
			}

			VertexAttribute(std::string name, VertexAttributeType type, int capacity) : IVertexAttribute(name, type), Values(std::list<T>(capacity))
			{
			}

		public:
			const std::list<T> Values;
		};*/

	}
}