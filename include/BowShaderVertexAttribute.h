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
		IntVector4,
		UnsignedInt,
		UnsignedIntVector2,
		UnsignedIntVector3,
		UnsignedIntVector4
	};

	struct ShaderVertexAttribute
	{
	public:
		ShaderVertexAttribute(int _Location, ShaderVertexAttributeType	_Type, int _Length) : SemanticName(""), SemanticIndex(-1), Location(_Location), Type(_Type), Length(_Length){}
		ShaderVertexAttribute(const char* _semanticName, int _semanticIndex, int _Location, ShaderVertexAttributeType	_Type, int _Length) : SemanticName(_semanticName), SemanticIndex(_semanticIndex), Location(_Location), Type(_Type), Length(_Length) {}
		~ShaderVertexAttribute(){}

		// For DirectX
		const std::string	SemanticName;
		const int			SemanticIndex;

		// For OpenGL
		const int			Location;

		const ShaderVertexAttributeType		Type;
		const int							Length;
	};


}
