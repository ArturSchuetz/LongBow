/**
 * @file IBowVertexAttribute.h
 * @brief Declarations for IBowVertexAttribute.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace bow {

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
		virtual ~IVertexAttribute() {}

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
		std::vector<T> Values;
	};


	typedef std::shared_ptr<IVertexAttribute> VertexAttributePtr;
	typedef std::unordered_map<std::string, VertexAttributePtr> VertexAttributeMap;

}
