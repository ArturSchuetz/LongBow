#include "BowSphereTessellator.h"

#include "BowMesh.h"
#include "BowIndicesUnsignedInt.h"
#include "BowVertexAttributeFloat.h"
#include "BowVertexAttributeFloatVec2.h"
#include "BowVertexAttributeFloatVec3.h"
#include "BowTriangleIndicesUnsignedInt.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Bow {
	namespace Core {
		
		Mesh SubdivisionSphereTessellator::ComputeFromTetrahedron(int numberOfSubdivisions, bool normalize)
		{
			LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

			Mesh mesh;

			VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3("in_Position");
			mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

			VertexAttributeFloatVec2 *texturCoordinatesAttribute = new VertexAttributeFloatVec2("in_TexCoord");
			mesh.AddAttribute(VertexAttributePtr(texturCoordinatesAttribute));

			IndicesUnsignedInt *indices = new IndicesUnsignedInt();
			mesh.Indices = IndicesBasePtr(indices);

			const double negativeRootTwoOverThree = -sqrt(2.0) / 3.0;
			const double negativeOneThird = -1.0 / 3.0;
			const double rootSixOverThree = sqrt(6.0) / 3.0;

			positionsAttribute->Values.push_back(Vector3<float>(0, 0, 1));
			positionsAttribute->Values.push_back(Vector3<float>(0, (float)((2.0 * sqrt(2.0)) / 3.0), (float)negativeOneThird));
			positionsAttribute->Values.push_back(Vector3<float>((float)-rootSixOverThree, (float)negativeRootTwoOverThree, (float)negativeOneThird));
			positionsAttribute->Values.push_back(Vector3<float>((float)rootSixOverThree, (float)negativeRootTwoOverThree, (float)negativeOneThird));
			
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[0]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[1]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[2]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[3]));

			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 1, 2), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 2, 3), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 3, 1), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 3, 2), numberOfSubdivisions, normalize);
			
			return mesh;
		}

		Mesh SubdivisionSphereTessellator::ComputeFromCube(int numberOfSubdivisions, bool normalize)
		{
			LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

			Mesh mesh;

			VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3("in_Position");
			mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

			VertexAttributeFloatVec2 *texturCoordinatesAttribute = new VertexAttributeFloatVec2("in_TexCoord");
			mesh.AddAttribute(VertexAttributePtr(texturCoordinatesAttribute));

			IndicesUnsignedInt *indices = new IndicesUnsignedInt();
			mesh.Indices = IndicesBasePtr(indices);

			Vector3<float> v0 = Vector3<float>(-1.0, -1.0, -1.0); v0.Normalize();
			Vector3<float> v1 = Vector3<float>(1.0, -1.0, -1.0); v1.Normalize();
			Vector3<float> v2 = Vector3<float>(1.0, 1.0, -1.0); v2.Normalize();
			Vector3<float> v3 = Vector3<float>(-1.0, 1.0, -1.0); v3.Normalize();
			Vector3<float> v4 = Vector3<float>(-1.0, -1.0, 1.0); v4.Normalize();
			Vector3<float> v5 = Vector3<float>(1.0, -1.0, 1.0); v5.Normalize();
			Vector3<float> v6 = Vector3<float>(1.0, 1.0, 1.0); v6.Normalize();
			Vector3<float> v7 = Vector3<float>(-1.0, 1.0, 1.0); v7.Normalize();

			positionsAttribute->Values.push_back(v0);
			positionsAttribute->Values.push_back(v1);
			positionsAttribute->Values.push_back(v2);
			positionsAttribute->Values.push_back(v3);
			positionsAttribute->Values.push_back(v4);
			positionsAttribute->Values.push_back(v5);
			positionsAttribute->Values.push_back(v6);
			positionsAttribute->Values.push_back(v7);

			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[0]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[1]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[2]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[3]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[4]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[5]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[6]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[7]));

			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(4, 5, 6), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(4, 6, 7), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 0, 3), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 3, 2), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 6, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 2, 6), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(2, 3, 7), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(2, 7, 6), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(3, 0, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(3, 4, 7), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 1, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 5, 4), numberOfSubdivisions, normalize);

			return mesh;
		}

		Mesh SubdivisionSphereTessellator::ComputeFromOctahedron(int numberOfSubdivisions, bool normalize)
		{
			LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

			Mesh mesh;

			VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3("in_Position");
			mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

			VertexAttributeFloatVec2 *texturCoordinatesAttribute = new VertexAttributeFloatVec2("in_TexCoord");
			mesh.AddAttribute(VertexAttributePtr(texturCoordinatesAttribute));

			IndicesUnsignedInt *indices = new IndicesUnsignedInt();
			mesh.Indices = IndicesBasePtr(indices);

			Vector3<float> v0 = Vector3<float>(1.0, 0.0, 0.0); v0.Normalize();
			Vector3<float> v1 = Vector3<float>(0.0, -1.0, 0.0); v1.Normalize();
			Vector3<float> v2 = Vector3<float>(-1.0, 0.0, 0.0); v2.Normalize();
			Vector3<float> v3 = Vector3<float>(0.0, 1.0, 0.0); v3.Normalize();
			Vector3<float> v4 = Vector3<float>(0.0, 0.0, 1.0); v4.Normalize();
			Vector3<float> v5 = Vector3<float>(0.0, 0.0, -1.0); v5.Normalize();

			positionsAttribute->Values.push_back(v0);
			positionsAttribute->Values.push_back(v1);
			positionsAttribute->Values.push_back(v2);
			positionsAttribute->Values.push_back(v3);
			positionsAttribute->Values.push_back(v4);
			positionsAttribute->Values.push_back(v5);

			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[0]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[1]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[2]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[3]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[4]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[5]));

			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 0, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(2, 1, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(3, 2, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 3, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 1, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(1, 2, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(2, 3, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(3, 0, 5), numberOfSubdivisions, normalize);

			return mesh;
		}

		Mesh SubdivisionSphereTessellator::ComputeFromIcosahedron(int numberOfSubdivisions, bool normalize)
		{
			LOG_ASSERT(numberOfSubdivisions >= 0, "numberOfSubdivisions has to be 0 or bigger");

			Mesh mesh;

			VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3("in_Position");
			mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

			VertexAttributeFloatVec2 *texturCoordinatesAttribute = new VertexAttributeFloatVec2("in_TexCoord");
			mesh.AddAttribute(VertexAttributePtr(texturCoordinatesAttribute));

			IndicesUnsignedInt *indices = new IndicesUnsignedInt();
			mesh.Indices = IndicesBasePtr(indices);

			const double X = 0.525731112119133606;
			const double Z = 0.850650808352039932;

			Vector3<double> v0 = Vector3<double>(-X, 0.0, Z); v0.Normalize();
			Vector3<double> v1 = Vector3<double>(X, 0.0, Z); v1.Normalize();
			Vector3<double> v2 = Vector3<double>(-X, 0.0, -Z); v2.Normalize();
			Vector3<double> v3 = Vector3<double>(X, 0.0, -Z); v3.Normalize();
			Vector3<double> v4 = Vector3<double>(0.0, Z, X); v4.Normalize();
			Vector3<double> v5 = Vector3<double>(0.0, Z, -X); v5.Normalize();
			Vector3<double> v6 = Vector3<double>(0.0, -Z, X); v6.Normalize();
			Vector3<double> v7 = Vector3<double>(0.0, -Z, -X); v7.Normalize();
			Vector3<double> v8 = Vector3<double>(Z, X, 0.0); v8.Normalize();
			Vector3<double> v9 = Vector3<double>(-Z, X, 0.0); v9.Normalize();
			Vector3<double> v10 = Vector3<double>(Z, -X, 0.0); v10.Normalize();
			Vector3<double> v11 = Vector3<double>(-Z, -X, 0.0); v11.Normalize();

			positionsAttribute->Values.push_back(v0);
			positionsAttribute->Values.push_back(v1);
			positionsAttribute->Values.push_back(v2);
			positionsAttribute->Values.push_back(v3);
			positionsAttribute->Values.push_back(v4);
			positionsAttribute->Values.push_back(v5);
			positionsAttribute->Values.push_back(v6);
			positionsAttribute->Values.push_back(v7);
			positionsAttribute->Values.push_back(v8);
			positionsAttribute->Values.push_back(v9);
			positionsAttribute->Values.push_back(v10);
			positionsAttribute->Values.push_back(v11);

			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[0]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[1]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[2]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[3]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[4]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[5]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[6]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[7]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[8]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[9]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[10]));
			texturCoordinatesAttribute->Values.push_back(ComputeTextureCoordinate(positionsAttribute->Values[11]));

			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 1, 4), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 4, 9), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(9, 4, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(4, 8, 5), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(4, 1, 8), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(8, 1, 10), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(8, 10, 3), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(5, 8, 3), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(5, 3, 2), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(2, 3, 7), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(7, 3, 10), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(7, 10, 6), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(7, 6, 11), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(11, 6, 0), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(0, 6, 1), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(6, 10, 1), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(9, 11, 0), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(9, 2, 11), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(9, 5, 2), numberOfSubdivisions, normalize);
			Subdivide(&(positionsAttribute->Values), &(texturCoordinatesAttribute->Values), indices, TriangleIndicesUnsignedInt(7, 11, 2), numberOfSubdivisions, normalize);

			return mesh;
		}

		void SubdivisionSphereTessellator::Subdivide(std::vector<Vector3<float>> *positions, std::vector<Vector2<float>> *texcoordinates, IndicesUnsignedInt *indices, TriangleIndicesUnsignedInt triangle, int level, bool normalize)
		{
			if (level > 0)
			{
				Vector3<float> vec1 = ((*positions)[triangle.GetIndex(0)] + (*positions)[triangle.GetIndex(1)]) * 0.5;
				Vector3<float> vec2 = ((*positions)[triangle.GetIndex(1)] + (*positions)[triangle.GetIndex(2)]) * 0.5;
				Vector3<float> vec3 = ((*positions)[triangle.GetIndex(2)] + (*positions)[triangle.GetIndex(0)]) * 0.5;

				// Normalize to make it a sphere
				vec1 = normalize ? vec1.Normalized() : vec1;
				vec2 = normalize ? vec2.Normalized() : vec2;
				vec3 = normalize ? vec3.Normalized() : vec3;

				positions->push_back(vec1);
				positions->push_back(vec2);
				positions->push_back(vec3);

				int i1 = positions->size() - 3;
				int i2 = positions->size() - 2;
				int i3 = positions->size() - 1;

				texcoordinates->push_back(ComputeTextureCoordinate(vec1));
				texcoordinates->push_back(ComputeTextureCoordinate(vec2));
				texcoordinates->push_back(ComputeTextureCoordinate(vec3));

				//
				// Subdivide input triangle into four triangles
				//
				--level;
				SubdivisionSphereTessellator::Subdivide(positions, texcoordinates, indices, TriangleIndicesUnsignedInt(triangle.GetIndex(0), i1, i3), level, normalize);
				SubdivisionSphereTessellator::Subdivide(positions, texcoordinates, indices, TriangleIndicesUnsignedInt(i1, triangle.GetIndex(1), i2), level, normalize);
				SubdivisionSphereTessellator::Subdivide(positions, texcoordinates, indices, TriangleIndicesUnsignedInt(i1, i2, i3), level, normalize);
				SubdivisionSphereTessellator::Subdivide(positions, texcoordinates, indices, TriangleIndicesUnsignedInt(i3, i2, triangle.GetIndex(2)), level, normalize);
			}
			else
			{
				indices->Values.push_back(triangle.GetIndex(0));
				indices->Values.push_back(triangle.GetIndex(1));
				indices->Values.push_back(triangle.GetIndex(2));
			}
		}

		Vector2<float> SubdivisionSphereTessellator::ComputeTextureCoordinate(Vector3<float> position)
		{
			return Vector2<float>(
				(float)((atan2((double)position.y, (double)position.x)) * (1.0 / (2.0 * M_PI)) + 0.5),
				(float)(asin((double)position.z) * (1.0 / M_PI) + 0.5)
			);
		}
	}
}