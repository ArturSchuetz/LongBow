#pragma once

namespace Bow {
	namespace Core {
		
		template<class T> class Vector2;
		template<class T> class Vector3;
			typedef Vector3<float> ColorRGB;
		template<class T> class Vector4;
			typedef Vector4<float> ColorRGBA;

		template <typename T> class Matrix2D;
		template <typename T> class Matrix2x2;
		template <typename T> class Matrix3D;
		template <typename T> class Matrix4x4;

		template <typename T> class Plane;
		template <typename T> class Ray;
		template <typename T> class Sphere;
		template <typename T> class Triangle;
		template <typename T> class Frustum;

		class EventLogger;
		class MemoryManager;

		enum class IndicesType : char;
		struct IndicesUnsignedInt;
		struct IndicesUnsignedShort;
		struct TriangleIndicesUnsignedInt;
		struct VertexAttributeFloat;
		struct VertexAttributeFloatVec2;
		struct VertexAttributeFloatVec3;
		struct VertexAttributeFloatVec4;

		class MeshAttribute;
			typedef std::shared_ptr<MeshAttribute> MeshAttributePtr;
	}
}
