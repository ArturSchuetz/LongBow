#pragma once

namespace Bow {
	namespace Core {
		
		class Bitmap;
		class Mesh;
			typedef std::shared_ptr<Mesh> MeshPtr;
		class SubMesh;
			typedef std::shared_ptr<SubMesh> SubMeshPtr;
		class ResourceManager;

		template<class T>
		class Vector2;
		template<class T>
		class Vector3;
		template<class T>
		class Vector4;

		class EventLogger;

		enum class IndicesType : char;
		struct IndicesUnsignedInt;
		struct IndicesUnsignedShort;
		struct VertexAttributeFloat;
		struct VertexAttributeFloatVec3;
	}
}
