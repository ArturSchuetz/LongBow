#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

namespace Bow {
	namespace Core {

		class SubMesh
		{
			friend class ResourceManager;

		public:
			~SubMesh();

			const int StartIndex;
			const int FaceCount;
			unsigned int NumIndices;

			const int MaterialID;
			const Mesh* Parent;

		private:
			SubMesh(Mesh* parent, int startIndex, int faceCount, int material) :
				Parent(parent),
				StartIndex(startIndex),
				FaceCount(faceCount),
				MaterialID(material)
			{} // I don't want anybody to create submeshes without parents
		};

	}
}