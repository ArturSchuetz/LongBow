#include "Resources/Resources/BowMesh.h"

#include "Resources/FileLoader/MeshLoader/BowModelLoader_obj.h"
#include "Resources/FileLoader/MeshLoader/BowModelLoader_ply.h"

#include "CoreSystems/BowMath.h"
#include "CoreSystems/Geometry/BowMeshAttribute.h"
#include "CoreSystems/Geometry/Indices/BowIndicesUnsignedInt.h"
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloatVec2.h"
#include "CoreSystems/Geometry/VertexAttributes/BowVertexAttributeFloatVec3.h"

#include "Platform/BowFileReader.h"

#include <iostream>
#include <limits>

namespace bow
{

Mesh::Mesh(ResourceManager *creator, const std::string &name, ResourceHandle handle)
    : Resource(creator, name, handle), m_dataFromDisk(nullptr), m_boundingBoxMax(Vector3<float>(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())),
      m_boundingBoxMin(Vector3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()))
{

    FN("Mesh::Mesh");

    LOG_TRACE("Creating Mesh: %s", m_name.c_str());
}

Mesh::~Mesh()
{
    FN("Mesh::~Mesh");

    // have to call this here reather than in Resource destructor
    // since calling virtual methods in base destructors causes crash
    VUnload();
}

MeshAttribute Mesh::CreateAttribute(const std::string &positionAttribute)
{
    FN("Mesh::CreateAttribute");
    OPTICK_EVENT();

    MeshAttribute mesh;

    // Add Positions
    VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), (int)m_vertices.size());
    positionsAttribute->Values = m_vertices;
    mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

    // Add Indices
    IndicesUnsignedInt *indices = new IndicesUnsignedInt((int)m_indices.size());
    indices->Values = m_indices;
    mesh.Indices = IndicesBasePtr(indices);

    return mesh;
}

MeshAttribute Mesh::CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute)
{
    FN("Mesh::CreateAttribute");
    OPTICK_EVENT();

    MeshAttribute mesh;

    // Add Positions
    VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), (int)m_vertices.size());
    positionsAttribute->Values = m_vertices;
    mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

    // Add Normals
    VertexAttributeFloatVec3 *normalsAttribute = new VertexAttributeFloatVec3(normalAttribute.c_str(), (int)m_normals.size());
    normalsAttribute->Values = m_normals;
    mesh.AddAttribute(VertexAttributePtr(normalsAttribute));

    // Add Indices
    IndicesUnsignedInt *indices = new IndicesUnsignedInt((int)m_indices.size());
    indices->Values = m_indices;
    mesh.Indices = IndicesBasePtr(indices);

    return mesh;
}

MeshAttribute Mesh::CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &textureCoordinateAttribute)
{
    FN("Mesh::CreateAttribute");
    OPTICK_EVENT();

    MeshAttribute mesh;

    // Add Positions
    VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), (int)m_vertices.size());
    positionsAttribute->Values = m_vertices;
    mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

    // Add Normals
    VertexAttributeFloatVec3 *normalsAttribute = new VertexAttributeFloatVec3(normalAttribute.c_str(), (int)m_normals.size());
    normalsAttribute->Values = m_normals;
    mesh.AddAttribute(VertexAttributePtr(normalsAttribute));

    if (HasTextureCoordinates())
    {
        // Add Texture Coordinates
        VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2(textureCoordinateAttribute.c_str(), (int)m_texCoords.size());
        texCoordAttribute->Values = m_texCoords;
        mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));
    }

    // Add Indices
    IndicesUnsignedInt *indices = new IndicesUnsignedInt((int)m_indices.size());
    indices->Values = m_indices;
    mesh.Indices = IndicesBasePtr(indices);

    return mesh;
}

MeshAttribute Mesh::CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &tangentAttribute, const std::string &bitangentAttribute, const std::string &textureCoordinateAttribute)
{
    FN("Mesh::CreateAttribute");
    OPTICK_EVENT();

    MeshAttribute mesh;

    // Add Positions
    VertexAttributeFloatVec3 *positionsAttribute = new VertexAttributeFloatVec3(positionAttribute.c_str(), (int)m_vertices.size());
    positionsAttribute->Values = m_vertices;
    mesh.AddAttribute(VertexAttributePtr(positionsAttribute));

    // Add Normals
    VertexAttributeFloatVec3 *normalsAttribute = new VertexAttributeFloatVec3(normalAttribute.c_str(), (int)m_normals.size());
    normalsAttribute->Values = m_normals;
    mesh.AddAttribute(VertexAttributePtr(normalsAttribute));

    if (HasTextureCoordinates())
    {
        VertexAttributeFloatVec3 *tangentsAttribute = new VertexAttributeFloatVec3(tangentAttribute.c_str(), (int)m_tangents.size());
        tangentsAttribute->Values = m_tangents;
        mesh.AddAttribute(VertexAttributePtr(tangentsAttribute));

        VertexAttributeFloatVec3 *bitangentsAttribute = new VertexAttributeFloatVec3(bitangentAttribute.c_str(), (int)m_bitangents.size());
        bitangentsAttribute->Values = m_bitangents;
        mesh.AddAttribute(VertexAttributePtr(bitangentsAttribute));

        // Add Texture Coordinates
        VertexAttributeFloatVec2 *texCoordAttribute = new VertexAttributeFloatVec2(textureCoordinateAttribute.c_str(), (int)m_texCoords.size());
        texCoordAttribute->Values = m_texCoords;
        mesh.AddAttribute(VertexAttributePtr(texCoordAttribute));
    }

    // Add Indices
    IndicesUnsignedInt *indices = new IndicesUnsignedInt((int)m_indices.size());
    indices->Values = m_indices;
    mesh.Indices = IndicesBasePtr(indices);

    return mesh;
}

SubMesh *Mesh::CreateSubMesh()
{
    FN("Mesh::CreateSubMesh");

    SubMesh *sub = new SubMesh();
    sub->m_parent = this;

    m_subMeshList.push_back(sub);

    if (VIsLoaded())
        _dirtyState();

    return sub;
}

SubMesh *Mesh::CreateSubMesh(const std::string &name)
{
    FN("Mesh::CreateSubMesh");

    SubMesh *sub = CreateSubMesh();
    NameSubMesh(name, (uint16_t)m_subMeshList.size() - 1);
    return sub;
}

void Mesh::NameSubMesh(const std::string &name, uint16_t index)
{
    FN("Mesh::NameSubMesh");

    m_subMeshNameMap[name] = index;
}

void Mesh::UnnameSubMesh(const std::string &name)
{
    FN("Mesh::UnnameSubMesh");

    auto i = m_subMeshNameMap.find(name);
    if (i != m_subMeshNameMap.end())
        m_subMeshNameMap.erase(i);
}

uint16_t Mesh::GetSubMeshIndex(const std::string &name) const
{
    FN("Mesh::GetSubMeshIndex");

    auto i = m_subMeshNameMap.find(name);
    if (i == m_subMeshNameMap.end())
    {
        LOG_ERROR("Mesh::GetSubMeshIndex: No SubMesh named '%s' found.", name);
    }

    return i->second;
}

SubMesh *Mesh::GetSubMesh(const std::string &name) const
{
    FN("Mesh::GetSubMesh");

    uint16_t index = GetSubMeshIndex(name);
    return GetSubMesh(index);
}

void Mesh::DestroySubMesh(uint16_t index)
{
    FN("Mesh::DestroySubMesh");
    OPTICK_EVENT();

    if (index >= (uint16_t)m_subMeshList.size())
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
    FN("Mesh::CalculateMissingNormals");
    OPTICK_EVENT();

    for (unsigned long i = 0; i + 2 < (unsigned long)m_indices.size(); i += 3)
    {
        while (m_normals.size() <= m_indices[i + 1] || m_normals.size() <= m_indices[i] || m_normals.size() <= m_indices[i + 2])
        {
            m_normals.push_back(Vector3<float>(0.0f, 0.0f, 0.0f));
        }

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

void Mesh::CalculateTangents()
{
    FN("Mesh::CalculateTangents");
    OPTICK_EVENT();

    uint32_t vertexCount = (uint32_t)m_vertices.size();
    uint32_t indexCount = (uint32_t)m_indices.size();
    m_tangents.resize(vertexCount);
    m_bitangents.resize(vertexCount);

    for (size_t a = 0; a + 2 < indexCount; a += 3)
    {
        long i1 = m_indices[a + 0];
        long i2 = m_indices[a + 1];
        long i3 = m_indices[a + 2];

        const Vector3<float> &v0 = m_vertices[i1];
        const Vector3<float> &v1 = m_vertices[i2];
        const Vector3<float> &v2 = m_vertices[i3];

        if (m_texCoords.size() > i3)
        {
            const Vector2<float> &uv0 = m_texCoords[i1];
            const Vector2<float> &uv1 = m_texCoords[i2];
            const Vector2<float> &uv2 = m_texCoords[i3];

            // Edges of the triangle : position delta
            const Vector3<float> &deltaPos1 = v1 - v0;
            const Vector3<float> &deltaPos2 = v2 - v0;

            // UV delta
            const Vector2<float> &deltaUV1 = uv1 - uv0;
            const Vector2<float> &deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

            Vector3<float> normal = (deltaPos1.CrossP(deltaPos2)).Normalized();
            Vector3<float> tangent = ((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r).Normalized();
            Vector3<float> bitangent = ((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r).Normalized() * -1.0f;

            // m_normals[i1] = normal;
            // m_normals[i2] = normal;
            // m_normals[i3] = normal;

            m_tangents[i1] = tangent;
            m_tangents[i2] = tangent;
            m_tangents[i3] = tangent;

            m_bitangents[i1] = bitangent;
            m_bitangents[i2] = bitangent;
            m_bitangents[i3] = bitangent;
        }
        else
        {
            const Vector3<float> &deltaPos1 = v1 - v0;
            const Vector3<float> &deltaPos2 = v2 - v0;

            Vector3<float> normal = (deltaPos1.CrossP(deltaPos2)).Normalized();
            Vector3<float> tangent = (deltaPos1).Normalized();
            Vector3<float> bitangent = (normal.CrossP(tangent)).Normalized();

            // m_normals[i1] = normal;
            // m_normals[i2] = normal;
            // m_normals[i3] = normal;

            m_tangents[i1] = tangent;
            m_tangents[i2] = tangent;
            m_tangents[i3] = tangent;

            m_bitangents[i1] = bitangent;
            m_bitangents[i2] = bitangent;
            m_bitangents[i3] = bitangent;
        }
    }
}

void Mesh::CalculateBoundingBox()
{
    FN("Mesh::CalculateBoundingBox");
    OPTICK_EVENT();

    m_boundingBoxMin = Vector3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    m_boundingBoxMax = Vector3<float>(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (size_t i = 0; i < m_vertices.size(); i++)
    {
        if (m_vertices[i].x < m_boundingBoxMin.x)
        {
            m_boundingBoxMin.x = m_vertices[i].x;
        }
        if (m_vertices[i].y < m_boundingBoxMin.y)
        {
            m_boundingBoxMin.y = m_vertices[i].y;
        }
        if (m_vertices[i].z < m_boundingBoxMin.z)
        {
            m_boundingBoxMin.z = m_vertices[i].z;
        }

        if (m_vertices[i].x > m_boundingBoxMax.x)
        {
            m_boundingBoxMax.x = m_vertices[i].x;
        }
        if (m_vertices[i].y > m_boundingBoxMax.y)
        {
            m_boundingBoxMax.y = m_vertices[i].y;
        }
        if (m_vertices[i].z > m_boundingBoxMax.z)
        {
            m_boundingBoxMax.z = m_vertices[i].z;
        }
    }
}

// ============================================================

void Mesh::VPrepareImpl()
{
    FN("Mesh::VPrepareImpl");

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
            readedBytes = (uint32_t)reader.Read(buffer, 1024);
            memcpy(m_dataFromDisk + i, buffer, readedBytes);
        }

        reader.Close();
    }
    else
    {
        LOG_ERROR("Could not open File '%s'!", filePath.c_str());
    }
}

void Mesh::VUnprepareImpl()
{
    FN("Mesh::VUnprepareImpl");

    if (m_dataFromDisk != nullptr)
    {
        delete[] m_dataFromDisk;
        m_dataFromDisk = nullptr;
    }
}

void Mesh::VLoadImpl()
{
    FN("Mesh::VLoadImpl");

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

        if (extension == "obj")
        {
            ModelLoader_obj loader;
            loader.ImportMesh(m_dataFromDisk, this);
        }
        else if (extension == "ply")
        {
            ModelLoader_ply loader;
            loader.ImportMesh(m_dataFromDisk, this);
        }
        else
        {
            LOG_ERROR("Unknown file format for mesh");
            return;
        }

        CalculateMissingNormals();

        if (HasTextureCoordinates())
        {
            CalculateTangents();
        }

        CalculateBoundingBox();
    }
}

void Mesh::VPostLoadImpl() { FN("Mesh::VPostLoadImpl"); }

void Mesh::VUnloadImpl()
{
    FN("Mesh::VUnloadImpl");

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

} // namespace bow
