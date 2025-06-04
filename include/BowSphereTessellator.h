/**
 * @file BowSphereTessellator.h
 * @brief Declarations for BowSphereTessellator.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace bow {

	class SubdivisionSphereTessellator
	{
	public:
		static MeshAttribute ComputeFromTetrahedron(int numberOfSubdivisions, bool normalize = true);
		static MeshAttribute ComputeFromCube(int numberOfSubdivisions, bool normalize = true);
		static MeshAttribute ComputeFromOctahedron(int numberOfSubdivisions, bool normalize = true);
		static MeshAttribute ComputeFromIcosahedron(int numberOfSubdivisions, bool normalize = true);

	private:
		static void Subdivide(std::vector<Vector3<float>> *positions, std::vector<Vector2<float>> *texcoordinates, IndicesUnsignedInt *indices, TriangleIndicesUnsignedInt triangle, int level, bool normalize);

		static Vector2<float> ComputeTextureCoordinate(Vector3<float> position);
	};
	/*----------------------------------------------------------------*/
}
