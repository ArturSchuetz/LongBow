#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow 
{
	namespace Core 
	{

		class SubMesh
		{
			friend class ResourceManager;

		public:
			~SubMesh();

			const int StartIndex;
			const int TriangleCount;

			const int MaterialID;
			const Mesh* Parent;

		private:
			SubMesh(Mesh* parent, int startIndex, int triangleCount, int material) :
				Parent(parent),
				StartIndex(startIndex),
				TriangleCount(triangleCount),
				MaterialID(material)
			{} // I don't want anybody to create submeshes without parents
		};

	}
}