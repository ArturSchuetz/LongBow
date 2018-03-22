#include "BowPointCloud.h"

#include "BowResources.h"
#include "BowPlatform.h"
#include "BowCore.h"

#include "BowPointCloudLoader.h"

#undef max
#undef min

namespace bow {
	
	PointCloud::PointCloud(ResourceManager* creator, const std::string& name, ResourceHandle handle)
		: Resource(creator, name, handle)
		, m_dataFromDisk(nullptr)
	{
	}

	PointCloud::~PointCloud()
	{
		// have to call this here reather than in Resource destructor
		// since calling virtual methods in base destructors causes crash
		VUnload();
	}

	MeshAttribute PointCloud::CreateAttribute(const std::string& positionAttribute, const std::string& colorAttribute)
	{
		return CreateAttribute(positionAttribute, "", colorAttribute);
	}

	MeshAttribute PointCloud::CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute, const std::string& colorAttribute)
	{
		MeshAttribute mesh;

		// Add Positions
		VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), m_vertices.size());
		positionsAttribute->Values = m_vertices;
		mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

		// Add Normals
		VertexAttributeFloatVec3 *normalsAttribute = new VertexAttributeFloatVec3(normalAttribute.c_str(), m_vertices.size());
		normalsAttribute->Values = m_vertices;
		mesh.AddAttribute(VertexAttributePtr(normalsAttribute));

		// Add Colors
		VertexAttributeFloatVec3 *colorsAttribute = new VertexAttributeFloatVec3(colorAttribute.c_str(), m_colors.size());
		colorsAttribute->Values = m_colors;
		mesh.AddAttribute(VertexAttributePtr(colorsAttribute));

		return mesh;
	}

	AABB<float> PointCloud::GetBoundingBox()
	{
		float min_x = std::numeric_limits<float>::max(); float min_y = std::numeric_limits<float>::max(); float min_z = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::lowest(); float max_y = std::numeric_limits<float>::lowest(); float max_z = std::numeric_limits<float>::lowest();

		//if there are no points we return an empty bounding box
		if (m_vertices.empty())
		{
			return AABB<float>();
		}

		for (unsigned int i = 0; i < m_vertices.size(); ++i)
		{
			const Vector3<float>& pt = m_vertices[i]; //do not copy but get a reference to the i-th point in the vector  

			if (pt.x < min_x) 
				min_x = pt.x;
			else if (pt.x > max_x)
				max_x = pt.x;

			if (pt.y < min_y)
				min_y = pt.y;
			else if (pt.y > max_y)
				max_y = pt.y;

			if (pt.z < min_z)
				min_z = pt.z;
			else if (pt.z > max_z)
				max_z = pt.z;
		}

		return AABB<float>(Vector3<float>(min_x, min_y, min_z), Vector3<float>(max_x, max_y, max_z));
	}

	// ============================================================

	void PointCloud::VPrepareImpl(void)
	{
		// fully prebuffer into host RAM
		std::string filePath = VGetName();

		FileReader reader;

		if (reader.Open(filePath.c_str()))
		{
			size_t sizeInBytes = reader.GetSizeOfFile();
			m_dataFromDisk = new char[sizeInBytes];
			m_dataFromDisk[sizeInBytes - 1] = '\0';

			LOG_WARNING("FileSize: %d!", sizeInBytes);

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

	void PointCloud::VUnprepareImpl(void)
	{
		if (m_dataFromDisk != nullptr)
		{
			delete[] m_dataFromDisk;
			m_dataFromDisk = nullptr;
		}
	}

	void PointCloud::VLoadImpl(void)
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

			if (extension == "xyz") 
			{
				PointCloudLoader loader;
				loader.ImportPointCloud(m_dataFromDisk, this);

				m_colors.resize(m_vertices.size());
				for (unsigned int i = 0; i < m_colors.size(); i++)
				{
					m_colors[i] = bow::Vector3<float>(1.0f, 1.0f, 1.0f);
				}
			}
			else
			{
				LOG_ERROR("Unknown file format for mesh");
			}
		}
	}

	void PointCloud::VPostLoadImpl(void)
	{

	}

	void PointCloud::VUnloadImpl(void)
	{
		m_vertices.clear();
	}

	size_t PointCloud::VCalculateSize(void) const
	{
		// calculate GPU size
		size_t ret = 0;
			

		return ret;
	}

}
