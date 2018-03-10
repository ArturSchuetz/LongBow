#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A mesh represents geometry without any material.
	*/
	class SubMesh
	{
		friend class Mesh;
		friend class ModelLoader_OBJ;

	public:
		SubMesh();
		~SubMesh();

		unsigned int GetStartIndex();
		unsigned int GetNumIndices();

		/// Reference to parent Mesh (not a smart pointer so child does not keep parent alive).
		Mesh* m_parent;

	private:
		std::string m_name;
		unsigned int m_startIndex;
		unsigned int m_numIndices;
	};
}