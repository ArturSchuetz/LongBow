#include "BowAABB.h"

#include "BowMeshAttribute.h"
#include "BowIndicesUnsignedShort.h"
#include "BowVertexAttributeFloatVec3.h"

namespace bow
{
	MeshAttribute CreateDebugMesh(const AABB<float>& aabb, const std::string& vertexAttributeName)
	{
		MeshAttribute boxMesh;

		std::vector<Vector3<float>> boxVertices;
		boxVertices.push_back(Vector3<float>(aabb.min.x, aabb.min.y, aabb.min.z));
		boxVertices.push_back(Vector3<float>(aabb.min.x, aabb.min.y, aabb.max.z));
		boxVertices.push_back(Vector3<float>(aabb.min.x, aabb.max.y, aabb.min.z));
		boxVertices.push_back(Vector3<float>(aabb.min.x, aabb.max.y, aabb.max.z));

		boxVertices.push_back(Vector3<float>(aabb.max.x, aabb.min.y, aabb.min.z));
		boxVertices.push_back(Vector3<float>(aabb.max.x, aabb.min.y, aabb.max.z));
		boxVertices.push_back(Vector3<float>(aabb.max.x, aabb.max.y, aabb.min.z));
		boxVertices.push_back(Vector3<float>(aabb.max.x, aabb.max.y, aabb.max.z));

		IndicesUnsignedShort *indices = new IndicesUnsignedShort();
		indices->Values.push_back(0); indices->Values.push_back(1);
		indices->Values.push_back(0); indices->Values.push_back(2);
		indices->Values.push_back(2); indices->Values.push_back(3);
		indices->Values.push_back(1); indices->Values.push_back(3);

		indices->Values.push_back(4); indices->Values.push_back(5);
		indices->Values.push_back(4); indices->Values.push_back(6);
		indices->Values.push_back(6); indices->Values.push_back(7);
		indices->Values.push_back(5); indices->Values.push_back(7);

		indices->Values.push_back(0); indices->Values.push_back(4);
		indices->Values.push_back(1); indices->Values.push_back(5);
		indices->Values.push_back(2); indices->Values.push_back(6);
		indices->Values.push_back(3); indices->Values.push_back(7);

		VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(vertexAttributeName, boxVertices.size());
		positionsAttribute->Values = boxVertices;

		boxMesh.AddAttribute(VertexAttributePtr(positionsAttribute));
		boxMesh.Indices = IndicesBasePtr(indices);

		return boxMesh;
	}
}