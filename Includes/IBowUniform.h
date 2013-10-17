#pragma once
#include "LongBow.h"

namespace Bow {

	enum class UniformType : char
    {
		NONE,
        Int,
        Float,
        FloatVector2,
        FloatVector3,
        FloatVector4,
        IntVector2,
        IntVector3,
        IntVector4,
        Bool,
        BoolVector2,
        BoolVector3,
        BoolVector4,
        FloatMatrix22,
        FloatMatrix33,
        FloatMatrix44,
        Sampler1D,
        Sampler2D,
        Sampler2DRectangle,
        Sampler2DRectangleShadow,
        Sampler3D,
        SamplerCube,
        Sampler1DShadow,
        Sampler2DShadow,
        FloatMatrix23,
        FloatMatrix24,
        FloatMatrix32,
        FloatMatrix34,
        FloatMatrix42,
        FloatMatrix43,
        Sampler1DArray,
        Sampler2DArray,
        Sampler1DArrayShadow,
        Sampler2DArrayShadow,
        SamplerCubeShadow,
        IntSampler1D,
        IntSampler2D,
        IntSampler2DRectangle,
        IntSampler3D,
        IntSamplerCube,
        IntSampler1DArray,
        IntSampler2DArray,
        UnsignedIntSampler1D,
        UnsignedIntSampler2D,
        UnsignedIntSampler2DRectangle,
        UnsignedIntSampler3D,
        UnsignedIntSamplerCube,
        UnsignedIntSampler1DArray,
        UnsignedIntSampler2DArray
    };

    
	class IUniform
    {
	public:
		IUniform(UniformType _Datatype, int _Location ) : Datatype(_Datatype), Location(_Location) {}
		virtual ~IUniform(){};

		virtual void	SetValue(void* value, bool deletePointer = true) = 0;
		virtual void	SetArray(void* value, bool deleteArray = true) = 0;

		const UniformType	Datatype;
		const int			Location;
    };

	typedef std::shared_ptr<IUniform> UniformPtr;
	typedef std::hash_map<std::string, UniformPtr> UniformMap;

}