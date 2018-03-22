#pragma once
#include "BowPrerequisites.h"
#include "BowRendererPredeclares.h"

namespace bow {

	enum class ShaderVertexAttributeType : char
	{
		INVALID,
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

	typedef std::shared_ptr<struct ShaderVertexAttribute> ShaderVertexAttributePtr;

}
