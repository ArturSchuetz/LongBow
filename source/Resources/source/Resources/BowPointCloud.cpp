#include "Resources/Resources/BowPointCloud.h"

#include "Resources/FileLoader/PointCloudLoader/BowPointCloudLoader_bin.h"
#include "Resources/FileLoader/PointCloudLoader/BowPointCloudLoader_xcn.h"
#include "Resources/FileLoader/PointCloudLoader/BowPointCloudLoader_xtc.h"
#include "Resources/FileLoader/PointCloudLoader/BowPointCloudLoader_xyz.h"

#include "CoreSystems/Geometry/BowMeshAttribute.h"
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloatVec3.h"

#include "Platform/BowFileReader.h"


#undef max
#undef min

namespace bow
{

PointCloud::PointCloud(ResourceManager *creator, const std::string &name, ResourceHandle handle) : Resource(creator, name, handle), m_dataFromDisk(nullptr) { FN("PointCloud::PointCloud"); }

PointCloud::~PointCloud()
{
    FN("PointCloud::~PointCloud");

    // have to call this here reather than in Resource destructor
    // since calling virtual methods in base destructors causes crash
    VUnload();
}

MeshAttribute PointCloud::CreateAttribute(const std::string &positionAttribute, const std::string &colorAttribute)
{
    FN("PointCloud::CreateAttribute");
    OPTICK_EVENT();

    return CreateAttribute(positionAttribute, "", colorAttribute);
}

MeshAttribute PointCloud::CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &colorAttribute)
{
    FN("PointCloud::CreateAttribute");
    OPTICK_EVENT();

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
    FN("PointCloud::GetBoundingBox");
    OPTICK_EVENT();

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();

    // if there are no points we return an empty bounding box
    if (m_vertices.empty())
    {
        return AABB<float>();
    }

    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        const Vector3<float> &pt = m_vertices[i]; // do not copy but get a reference to the i-th point
                                                  // in the vector

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

void PointCloud::VPrepareImpl()
{
    FN("PointCloud::VPrepareImpl");

    // fully prebuffer into host RAM
    std::string filePath = VGetName();

    FileReader reader;

    if (reader.Open(filePath.c_str()))
    {
        m_sizeInBytes = reader.GetSizeOfFile();
        m_dataFromDisk = new char[m_sizeInBytes];
        m_dataFromDisk[m_sizeInBytes - 1] = '\0';

        uint32_t readedBytes = 0;
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

void PointCloud::VUnprepareImpl()
{
    FN("PointCloud::VUnprepareImpl");

    if (m_dataFromDisk != nullptr)
    {
        delete[] m_dataFromDisk;
        m_dataFromDisk = nullptr;
    }
}

void PointCloud::VLoadImpl()
{
    FN("PointCloud::VLoadImpl");

    if (m_dataFromDisk == nullptr)
    {
        LOG_ERROR("Data doesn't appear to have been prepared in %s !", VGetName().c_str());
        return;
    }

    std::string filePath = VGetName();
    size_t pos = filePath.find_last_of(".");
    if (pos >= 0)
    {
        std::string extension = filePath.substr(pos + 1);

        if (extension == "bin")
        {
            PointCloudLoader_bin loader;
            loader.ImportPointCloud(m_dataFromDisk, m_sizeInBytes, this);

            m_colors.resize(m_vertices.size());
            for (size_t i = 0; i < m_colors.size(); i++)
            {
                m_colors[i] = Vector3<float>(1.0f, 1.0f, 1.0f);
            }
        }
        else if (extension == "xyz")
        {
            PointCloudLoader_xyz loader;
            loader.ImportPointCloud(m_dataFromDisk, this);

            m_colors.resize(m_vertices.size());
            for (size_t i = 0; i < m_colors.size(); i++)
            {
                m_colors[i] = Vector3<float>(1.0f, 1.0f, 1.0f);
            }
        }
        else if (extension == "xtc")
        {
            PointCloudLoader_xtc loader;
            loader.ImportPointCloud(m_dataFromDisk, this);
        }
        else if (extension == "xcn")
        {
            PointCloudLoader_xcn loader;
            loader.ImportPointCloud(m_dataFromDisk, this);
        }
        else
        {
            LOG_ERROR("Unknown file format for pointcloud");
        }
    }
}

void PointCloud::VPostLoadImpl() { FN("PointCloud::VPostLoadImpl"); }

void PointCloud::VUnloadImpl()
{
    FN("PointCloud::VUnloadImpl");

    m_vertices.clear();
}

size_t PointCloud::VCalculateSize() const
{
    FN("PointCloud::VCalculateSize");

    // calculate GPU size
    size_t ret = 0;

    return ret;
}

} // namespace bow
