#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

#include "BowMesh.h"
#include "BowMeshAttribute.h"

namespace Bow {
	namespace Core {

		Mesh::Mesh(ResourceManager* creator, const std::string& name, ResourceHandle handle)
			: Resource(creator, name, handle)
			, m_dataFromDisk(nullptr)
		{
			LOG_TRACE("Creating Mesh: %s", name.c_str());
		}

		Mesh::~Mesh()
		{        
			// have to call this here reather than in Resource destructor
			// since calling virtual methods in base destructors causes crash
			VUnload();
		}

		MeshAttribute Mesh::CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute, const std::string& textureCoordinateAttribute)
		{
			MeshAttribute mesh;

			// Add Positions
			VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), m_vertices.size());
			positionsAttribute->Values = m_vertices;
			mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

			// Add Normals
			VertexAttributeFloatVec3 *normalsAttribute = new VertexAttributeFloatVec3(normalAttribute.c_str(), m_normals.size());
			normalsAttribute->Values = m_normals;
			mesh.AddAttribute(VertexAttributePtr(normalsAttribute));

			// Add Texture Coordinates
			VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2(textureCoordinateAttribute.c_str(), m_texCoords.size());
			texCoordAttribute->Values = m_texCoords;
			mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));

			// Add Indices
			IndicesUnsignedInt *indices = new IndicesUnsignedInt(m_indices.size());
			indices->Values = m_indices;
			mesh.Indices = IndicesBasePtr(indices);

			return mesh;
		}

		SubMesh* Mesh::CreateSubMesh(void)
		{
			SubMesh* sub = new SubMesh();
			sub->m_parent = this;

			m_subMeshList.push_back(sub);

			if (VIsLoaded())
				_dirtyState();

			return sub;
		}

		SubMesh* Mesh::CreateSubMesh(const std::string& name)
		{
			SubMesh *sub = CreateSubMesh();
			NameSubMesh(name, (unsigned short)m_subMeshList.size() - 1);
			return sub;
		}

		void Mesh::NameSubMesh(const std::string& name, unsigned short index)
		{
			m_subMeshNameMap[name] = index;
		}

		void Mesh::UnnameSubMesh(const std::string& name)
		{
			auto i = m_subMeshNameMap.find(name);
			if (i != m_subMeshNameMap.end())
				m_subMeshNameMap.erase(i);
		}

		unsigned short Mesh::GetSubMeshIndex(const std::string& name) const
		{
			auto i = m_subMeshNameMap.find(name);
			if (i == m_subMeshNameMap.end())
			{
				LOG_ERROR("Mesh::GetSubMeshIndex: No SubMesh named '%s' found.", name);
			}

			return i->second;
		}

		SubMesh* Mesh::GetSubMesh(const std::string& name) const
		{
			unsigned short index = GetSubMeshIndex(name);
			return GetSubMesh(index);
		}

		void Mesh::DestroySubMesh(unsigned short index)
		{
			if (index >= m_subMeshList.size())
			{
				LOG_ERROR("Mesh::DestroySubMesh: Index out of bounds.");
			}
			auto it = m_subMeshList.begin();
			std::advance(it, index);
			m_subMeshList.erase(it);

			// Fix up any name/index entries
			for (auto ni = m_subMeshNameMap.begin(); ni != m_subMeshNameMap.end();)
			{
				if (ni->second == index)
				{
					auto eraseIt = ni++;
					m_subMeshNameMap.erase(eraseIt);
				}
				else
				{
					// reduce indexes following
					if (ni->second > index)
						ni->second = ni->second - 1;

					++ni;
				}
			}

			if (VIsLoaded())
				_dirtyState();
		}

		// ============================================================

		void Mesh::VPrepareImpl(void)
		{
			// fully prebuffer into host RAM
			std::string filePath = VGetName();

			Platform::FileReader reader;

			if (reader.Open(filePath.c_str()))
			{
				size_t sizeInBytes = reader.GetSizeOfFile();
				m_dataFromDisk = new char[sizeInBytes];
				
				unsigned int readedBytes = 0;
				char buffer[1024];

				reader.Seek(0);
				for (size_t i = 0; !reader.EndOfFile(); i += readedBytes) 
				{
					readedBytes = reader.Read(buffer, 1024);
					memcpy(m_dataFromDisk + i, buffer, readedBytes);
				}
			}
			else
			{
				LOG_ERROR("Could not open File '%s'!", filePath.c_str());
			}
		}

		void Mesh::VUnprepareImpl(void)
		{
			if (m_dataFromDisk != nullptr)
			{
				delete[] m_dataFromDisk;
				m_dataFromDisk = nullptr;
			}
		}

		void Mesh::VLoadImpl(void)
		{
			if (m_dataFromDisk == nullptr)
			{
				LOG_ERROR("Data doesn't appear to have been prepared in %s !", VGetName().c_str());
				return;
			}

			std::string filePath = VGetName();
			size_t pos = filePath.find_last_of(".");
			if (pos >= 0)
			{
				std::string extension = filePath.substr(pos +1);

				if (extension == "obj") 
				{
					ModelLoader_OBJ loader;
					loader.ImportMesh(m_dataFromDisk, this);
				}
				else
				{
					LOG_ERROR("Unknown file format for mesh");
				}
			}
		}

		void Mesh::VPostLoadImpl(void)
		{

		}

		void Mesh::VUnloadImpl(void)
		{
			// Teardown submeshes
			for (auto i = m_subMeshList.begin(); i != m_subMeshList.end(); ++i)
			{
				delete *i;
			}

			// Clear SubMesh lists
			m_subMeshList.clear();
			m_subMeshNameMap.clear();

			m_vertices.clear();
			m_normals.clear();
			m_texCoords.clear();
		}

		size_t Mesh::VCalculateSize(void) const
		{
			// calculate GPU size
			size_t ret = 0;
			

			return ret;
		}

	}
}