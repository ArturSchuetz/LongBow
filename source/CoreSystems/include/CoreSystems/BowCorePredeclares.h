#pragma once
#include "CoreSystems/CoreSystems_api.h"

#include <memory>

namespace bow
{

template <class T> class Vector2;
template <class T> class Vector3;
template <class T> class Vector4;
typedef Vector3<float> ColorRGB;
typedef Vector4<float> ColorRGBA;

template <typename T> class Matrix;
template <typename T> class Matrix_trans;
template <typename T> class Matrix2D;
template <typename T> class Matrix3D;
template <typename T> class Matrix2x2;
template <typename T> class Matrix3x3;
template <typename T> class Matrix4x4;

template <typename T> class Quaternion;
template <typename T> class Transform;

template <typename T> class AABB;
template <typename T> class Plane;
template <typename T> class Ray;
template <typename T> class Sphere;
template <typename T> class Triangle;
template <typename T> class Frustum;

enum class IndicesType : char;
struct CORESYSTEMS_API IndicesUnsignedInt;
struct CORESYSTEMS_API IndicesUnsignedShort;
struct CORESYSTEMS_API TriangleIndicesUnsignedInt;
struct CORESYSTEMS_API VertexAttributeFloat;
struct CORESYSTEMS_API VertexAttributeFloatVec2;
struct CORESYSTEMS_API VertexAttributeFloatVec3;
struct CORESYSTEMS_API VertexAttributeFloatVec4;

class MeshAttribute;
typedef std::shared_ptr<MeshAttribute> MeshAttributePtr;

class SubdivisionSphereTessellator;

class CORESYSTEMS_API BasicTimer;
class CORESYSTEMS_API EventLogger;
class CORESYSTEMS_API Profiler;
class CORESYSTEMS_API Utils;

} // namespace bow
