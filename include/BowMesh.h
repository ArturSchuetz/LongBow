#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A sub mesh represents geometry without any material.
	*/
	class SubMesh
	{
		friend class Mesh;
		friend class ModelLoader_obj;

	public:
		SubMesh() : m_parent(0) {}
		~SubMesh(){}

		unsigned int GetStartIndex() {
			return m_startIndex;
		}

		unsigned int GetNumIndices() {
			return m_numIndices; 
		}

		const std::string& GetMaterialName() {
			return m_material;
		}

	private:
		/// Reference to parent Mesh (not a smart pointer so child does not keep parent alive).
		Mesh* m_parent;

		std::string m_name;
		unsigned int m_startIndex;
		unsigned int m_numIndices;

		std::string m_material;
	};

	// ---------------------------------------------------------------------------
	/** @brief A mesh represents geometry without any material.
	*/
	class Mesh : public Resource
	{
		friend class SubMesh;
		friend class ModelLoader_obj;

	public:
		Mesh(ResourceManager* creator, const std::string& name, ResourceHandle handle);
		~Mesh();

		MeshAttribute CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute, const std::string& textureCoordinateAttribute);

		/** Creates a new SubMesh.
		@remarks
		Method for manually creating geometry for the mesh.
		Note - use with extreme caution - you must be sure that
		you have set up the geometry properly.
		*/
		SubMesh* CreateSubMesh(void);

		/** Creates a new SubMesh and gives it a name
		*/
		SubMesh* CreateSubMesh(const std::string& name);

		/** Gives a name to a SubMesh
		*/
		void NameSubMesh(const std::string& name, unsigned short index);

		/** Removes a name from a SubMesh
		*/
		void UnnameSubMesh(const std::string& name);

		/** Gets the index of a submesh with a given name.
		@remarks
		Useful if you identify the SubMeshes by name (using nameSubMesh)
		but wish to have faster repeat access.
		*/
		unsigned short GetSubMeshIndex(const std::string& name) const;

		/** Gets the number of sub meshes which comprise this mesh.
		*  @deprecated use getSubMeshes() instead
		*/
		size_t GetNumSubMeshes(void) const {
			return m_subMeshList.size();
		}

		/** Gets a pointer to the submesh indicated by the index.
		*  @deprecated use getSubMeshes() instead
		*/
		SubMesh* GetSubMesh(size_t index) const {
			return m_subMeshList[index];
		}

		/** Gets a SubMesh by name
		*/
		SubMesh* GetSubMesh(const std::string& name) const;

		/** Destroy a SubMesh with the given index.
		@note
		This will invalidate the contents of any existing Entity, or
		any other object that is referring to the SubMesh list. Entity will
		detect this and reinitialise, but it is still a disruptive action.
		*/
		void DestroySubMesh(unsigned short index);

		/// Gets the available submeshes
		const std::vector<SubMesh*>& GetSubMeshes() const {
			return m_subMeshList;
		}

		const std::vector<std::string>& GetMaterialFiles() const {
			return m_materialFilesList;
		}

	private:
		/** Loads the mesh from disk.  This call only performs IO, it
			does not parse the bytestream or check for any errors therein.
			It also does not set up submeshes, etc.  You have to call load()
			to do that.
		*/
		void VPrepareImpl(void);

		/** Destroys data cached by prepareImpl.
		*/
		void VUnprepareImpl(void);

		/// @copydoc Resource::VLoadImpl
		void VLoadImpl(void);

		/// @copydoc Resource::VPostLoadImpl
		void VPostLoadImpl(void);

		/// @copydoc Resource::VUnloadImpl
		void VUnloadImpl(void);

		char* m_dataFromDisk;

		/** A list of submeshes which make up this mesh.
		Each mesh is made up of 1 or more submeshes, which
		are each based on a single material and can have their
		own vertex data (they may not - they can share vertex data
		from the Mesh, depending on preference).
		*/
		std::vector<SubMesh*>					m_subMeshList;
		std::map<std::string, unsigned short>	m_subMeshNameMap;

		std::vector<std::string>				m_materialFilesList;

		std::vector<unsigned int>	m_indices;
		std::vector<Vector3<float>> m_vertices;
		std::vector<Vector3<float>> m_normals;
		std::vector<Vector2<float>> m_texCoords;
	};
}
