#include "BowMesh.h"

#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

#include "BowMeshAttribute.h"

#include "BowModelLoader_obj.h"

namespace bow {

	Mesh::Mesh(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: Resource(creator, name, handle)
		, m_dataFromDisk(nullptr)
	{
		LOG_TRACE("Creating Mesh: %s", m_name.c_str());
	}

	Mesh::~Mesh()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		VUnload();
	}

	MeshAttribute Mesh::CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute)
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

		// Add Indices
		IndicesUnsignedInt *indices = new IndicesUnsignedInt(m_indices.size());
		indices->Values = m_indices;
		mesh.Indices = IndicesBasePtr(indices);

		return mesh;
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

		if (HasTextureCoordinates())
		{
			// Add Texture Coordinates
			VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2(textureCoordinateAttribute.c_str(), m_texCoords.size());
			texCoordAttribute->Values = m_texCoords;
			mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));
		}

		// Add Indices
		IndicesUnsignedInt *indices = new IndicesUnsignedInt(m_indices.size());
		indices->Values = m_indices;
		mesh.Indices = IndicesBasePtr(indices);

		return mesh;
	}

	MeshAttribute Mesh::CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute, const std::string& tangentAttribute, const std::string& bitangentAttribute, const std::string& textureCoordinateAttribute)
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

		if (HasTextureCoordinates())
		{
			VertexAttributeFloatVec3 *tangentsAttribute = new VertexAttributeFloatVec3(tangentAttribute.c_str(), m_tangents.size());
			tangentsAttribute->Values = m_tangents;
			mesh.AddAttribute(VertexAttributePtr(tangentsAttribute));

			VertexAttributeFloatVec3 *bitangentsAttribute = new VertexAttributeFloatVec3(bitangentAttribute.c_str(), m_bitangents.size());
			bitangentsAttribute->Values = m_bitangents;
			mesh.AddAttribute(VertexAttributePtr(bitangentsAttribute));

			// Add Texture Coordinates
			VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2(textureCoordinateAttribute.c_str(), m_texCoords.size());
			texCoordAttribute->Values = m_texCoords;
			mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));
		}

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


	void Mesh::CalculateMissingNormals()
	{
		for (unsigned long i = 0; i+2 < m_indices.size(); i += 3)
		{
			if (m_normals[m_indices[i]] == Vector3<float>(0.0f, 0.0f, 0.0f) || m_normals[m_indices[i + 1]] == Vector3<float>(0.0f, 0.0f, 0.0f) || m_normals[m_indices[i + 2]] == Vector3<float>(0.0f, 0.0f, 0.0f))
			{
				Vector3<float> v1 = m_vertices[m_indices[i]] - m_vertices[m_indices[i + 1]];
				Vector3<float> v2 = m_vertices[m_indices[i]] - m_vertices[m_indices[i + 2]];

				Vector3<float> normal = v1.CrossP(v2).Normalized();

				m_normals[m_indices[i]] = normal;
				m_normals[m_indices[i + 1]] = normal;
				m_normals[m_indices[i + 2]] = normal;
			}
			else
			{
				continue;
			}
		}
	}

	//*
	void Mesh::CalculateTangents()
	{
		unsigned int vertexCount = m_vertices.size();
		unsigned int indexCount = m_indices.size();
		m_tangents.resize(vertexCount);
		m_bitangents.resize(vertexCount);

		for (unsigned int a = 0; a + 2 < indexCount; a += 3)
		{
			long i1 = m_indices[a + 0];
			long i2 = m_indices[a + 1];
			long i3 = m_indices[a + 2];

			const Vector3<float>& v0 = m_vertices[i1];
			const Vector3<float>& v1 = m_vertices[i2];
			const Vector3<float>& v2 = m_vertices[i3];

			const Vector2<float>& uv0 = m_texCoords[i1];
			const Vector2<float>& uv1 = m_texCoords[i2];
			const Vector2<float>& uv2 = m_texCoords[i3];

			// Edges of the triangle : position delta
			const Vector3<float>& deltaPos1 = v1 - v0;
			const Vector3<float>& deltaPos2 = v2 - v0;

			// UV delta
			const Vector2<float>& deltaUV1 = uv1 - uv0;
			const Vector2<float>& deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			Vector3<float> normal = (deltaPos1.CrossP(deltaPos2)).Normalized();
			Vector3<float> tangent = ((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r).Normalized();
			Vector3<float> bitangent = ((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r).Normalized();

			//m_normals[i1] = normal;
			//m_normals[i2] = normal;
			//m_normals[i3] = normal;

			m_tangents[i1] = tangent;
			m_tangents[i2] = tangent;
			m_tangents[i3] = tangent;

			m_bitangents[i1] = bitangent;
			m_bitangents[i2] = bitangent;
			m_bitangents[i3] = bitangent;
		}

		//for (unsigned long a = 0; a < m_tangents.size(); a++)
		//{
		//	const Vector3<float>& tangent = m_tangents[a];

		//	// Gram-Schmidt orthogonalize
		//	m_tangents[a] = ((m_tangents[a] - m_normals[a]) * (m_normals[a].DotP(m_tangents[a]))).Normalized();

		//	// Calculate handedness
		//	if ((m_normals[a].CrossP(m_tangents[a])).DotP(m_bitangents[a]) < 0.0f)
		//	{
		//		m_tangents[a] = m_tangents[a] * -1.0f;
		//	}

		//	m_bitangents[a] = (m_normals[a].CrossP(m_tangents[a])).Normalized();
		//}
	}
	/*/
	void Mesh::CalculateTangents()
	{
		unsigned int vertexCount = m_vertices.size();
		unsigned int indexCount = m_indices.size();
		m_tangents.resize(vertexCount);
		m_bitangents.resize(vertexCount);

		Vector3<float> *tan1 = new Vector3<float>[vertexCount * 2];
		Vector3<float> *tan2 = tan1 + vertexCount;
		memset(tan1, 0, sizeof(Vector3<float>) * vertexCount * 2);

		for (unsigned long a = 0; a+2 < indexCount; a+=3)
		{
			long i1 = m_indices[a+0];
			long i2 = m_indices[a+1];
			long i3 = m_indices[a+2];

			const Vector3<float>& v1 = m_vertices[i1];
			const Vector3<float>& v2 = m_vertices[i2];
			const Vector3<float>& v3 = m_vertices[i3];

			const Vector2<float>& w1 = m_texCoords[i1];
			const Vector2<float>& w2 = m_texCoords[i2];
			const Vector2<float>& w3 = m_texCoords[i3];

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			Vector3<float> sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			Vector3<float> tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (unsigned long a = 0; a < vertexCount; a++)
		{
			const Vector3<float>& tangent = tan1[a];

			// Gram-Schmidt orthogonalize
			m_tangents[a] = ((tangent - m_normals[a]) * (m_normals[a].DotP(tangent))).Normalized();

			// Calculate handedness
			if ((m_normals[a].CrossP(tangent)).DotP(tan2[a]) < 0.0f)
			{
				m_tangents[a] = m_tangents[a] * -1.0f;
			}

			m_bitangents[a] = (m_normals[a].CrossP(m_tangents[a])).Normalized();
		}

		delete[] tan1;
	}
	//*/

	// ============================================================

	void Mesh::VPrepareImpl(void)
	{
		// fully prebuffer into host RAM
		std::string filePath = VGetName();

		FileReader reader;

		if (reader.Open(filePath.c_str()))
		{
			size_t sizeInBytes = reader.GetSizeOfFile();
			m_dataFromDisk = new char[sizeInBytes];
			m_dataFromDisk[sizeInBytes - 1] = '\0';
				
			unsigned int readedBytes = 0;
			char buffer[1024];

			reader.Seek(0);
			for (size_t i = 0; !reader.EndOfFile(); i += readedBytes) 
			{
				readedBytes = reader.Read(buffer, 1024);
				memcpy(m_dataFromDisk + i, buffer, readedBytes);
			}

			reader.Close();
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
				ModelLoader_obj loader;
				loader.ImportMesh(m_dataFromDisk, this);
			}
			else
			{
				LOG_ERROR("Unknown file format for mesh");
				return;
			}

			CalculateMissingNormals();
			
			CalculateTangents();
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

		m_indices.clear();
		m_vertices.clear();
		m_normals.clear();
		m_texCoords.clear();
	}
}
