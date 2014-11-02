#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

namespace Bow 
{
	namespace Core 
	{

		class SubMesh
		{
			friend class Model;
			friend class ModelLoaderOBJ;
		public:
			~SubMesh();

		private:
			SubMesh(){}				// I don't want anybody to create submeshes without parents
			SubMesh(Mesh* parent);	// also I don't want anybody to add a single submesh to multiple parent meshes

			Mesh* m_Parent;
		};

	}
}