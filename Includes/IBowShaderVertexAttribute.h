#pragma once
#include "LongBow.h"

namespace Bow {

	enum class ShaderVertexAttributeType : char
	{
		NONE,
		Float,
		FloatVector2,
		FloatVector3,
		FloatVector4,
		FloatMatrix22,
		FloatMatrix33,
		FloatMatrix44,
		Int,
		IntVector2,
		IntVector3,
		IntVector4
	};

	struct ShaderVertexAttribute
	{
	public:
		ShaderVertexAttribute(int _Location, ShaderVertexAttributeType	_Type, int _Length) : Location(_Location), Type(_Type), Length(_Length){}
		~ShaderVertexAttribute(){}

		const int							Location;
		const ShaderVertexAttributeType		Type;
		const int							Length;
	};

	typedef std::shared_ptr<ShaderVertexAttribute> ShaderVertexAttributePtr;
	typedef std::hash_map<std::string, ShaderVertexAttributePtr> ShaderVertexAttributeMap;

}