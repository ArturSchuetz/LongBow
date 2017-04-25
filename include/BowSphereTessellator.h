#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow {
	namespace Core {

		class SubdivisionSphereTessellator
		{
		public:
			static Mesh ComputeFromTetrahedron(int numberOfSubdivisions, bool normalize = true);
			static Mesh ComputeFromCube(int numberOfSubdivisions, bool normalize = true);
			static Mesh ComputeFromOctahedron(int numberOfSubdivisions, bool normalize = true);
			static Mesh ComputeFromIcosahedron(int numberOfSubdivisions, bool normalize = true);

		private:
			static void Subdivide(std::vector<Vector3<float>> *positions, std::vector<Vector2<float>> *texcoordinates, IndicesUnsignedInt *indices, TriangleIndicesUnsignedInt triangle, int level, bool normalize);

			static Vector2<float> ComputeTextureCoordinate(Vector3<float> position);
		};
		/*----------------------------------------------------------------*/
	}
}