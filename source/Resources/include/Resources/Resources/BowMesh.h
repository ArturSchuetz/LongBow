#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "CoreSystems/BowMath.h"
#include "Resources/BowResource.h"

#include <map>

namespace bow
{

// ---------------------------------------------------------------------------
/** @brief A sub mesh represents geometry without any material.
 */
class RESOURCES_API SubMesh
{
    friend class Mesh;
    friend class ModelLoader_obj;
    friend class ModelLoader_ply;

  public:
    SubMesh() : m_parent(nullptr), m_startIndex(0), m_numIndices(0) { FN("SubMesh::SubMesh"); }
    ~SubMesh() { FN("SubMesh::~SubMesh"); }

    uint32_t GetStartIndex()
    {
        FN("SubMesh::GetStartIndex");

        return m_startIndex;
    }

    uint32_t GetNumIndices()
    {
        FN("SubMesh::GetNumIndices");

        return m_numIndices;
    }

    const std::string &GetMaterialName()
    {
        FN("SubMesh::GetMaterialName");

        return m_material;
    }

    const void SetMaterialName(const std::string &material)
    {
        FN("SubMesh::SetMaterialName");

        m_material = material;
    }

  private:
    /// Reference to parent Mesh (not a smart pointer so child does not keep
    /// parent alive).
    Mesh *m_parent;

    std::string m_name;
    uint32_t m_startIndex;
    uint32_t m_numIndices;

    std::string m_material;
};

// ---------------------------------------------------------------------------
/** @brief A mesh represents geometry without any material.
 */
class RESOURCES_API Mesh : public Resource
{
    friend class SubMesh;
    friend class ModelLoader_obj;
    friend class ModelLoader_ply;

  public:
    Mesh(ResourceManager *creator, const std::string &name, ResourceHandle handle);
    ~Mesh();

    MeshAttribute CreateAttribute(const std::string &positionAttribute);
    MeshAttribute CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute);
    MeshAttribute CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &textureCoordinateAttribute);
    MeshAttribute CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &tangentAttribute, const std::string &bitangentAttribute, const std::string &textureCoordinateAttribute);

    /** Creates a new SubMesh.
    @remarks
    Method for manually creating geometry for the mesh.
    Note - use with extreme caution - you must be sure that
    you have set up the geometry properly.
    */
    SubMesh *CreateSubMesh();

    /** Creates a new SubMesh and gives it a name
     */
    SubMesh *CreateSubMesh(const std::string &name);

    /** Gives a name to a SubMesh
     */
    void NameSubMesh(const std::string &name, uint16_t index);

    /** Removes a name from a SubMesh
     */
    void UnnameSubMesh(const std::string &name);

    /** Gets the index of a submesh with a given name.
    @remarks
    Useful if you identify the SubMeshes by name (using nameSubMesh)
    but wish to have faster repeat access.
    */
    uint16_t GetSubMeshIndex(const std::string &name) const;

    /** Gets the number of sub meshes which comprise this mesh.
     *  @deprecated use getSubMeshes() instead
     */
    size_t GetNumSubMeshes() const
    {
        FN("Mesh::GetNumSubMeshes");

        return m_subMeshList.size();
    }

    /** Gets a pointer to the submesh indicated by the index.
     *  @deprecated use getSubMeshes() instead
     */
    SubMesh *GetSubMesh(size_t index) const
    {
        FN("Mesh::GetSubMesh");
        return m_subMeshList[index];
    }

    /** Gets a SubMesh by name
     */
    SubMesh *GetSubMesh(const std::string &name) const;

    /** Destroy a SubMesh with the given index.
    @note
    This will invalidate the contents of any existing Entity, or
    any other object that is referring to the SubMesh list. Entity will
    detect this and reinitialise, but it is still a disruptive action.
    */
    void DestroySubMesh(uint16_t index);

    /// Gets the available submeshes
    const std::vector<SubMesh *> &GetSubMeshes() const
    {
        FN("Mesh::GetSubMeshes");

        return m_subMeshList;
    }

    const std::vector<std::string> &GetMaterialFiles() const
    {
        FN("Mesh::GetMaterialFiles");

        return m_materialFilesList;
    }

    bool HasNormals()
    {
        FN("Mesh::HasNormals");

        return m_normals.size() > 0;
    }

    bool HasTextureCoordinates()
    {
        FN("Mesh::HasTextureCoordinates");

        return m_texCoords.size() > 0;
    }

    uint32_t GetNumIndices()
    {
        FN("Mesh::GetNumIndices");

        return m_indices.size();
    }

    uint32_t GetNumTriangles()
    {
        FN("Mesh::GetNumTriangles");

        return m_indices.size() / 3;
    }

    uint32_t GetNumVertices()
    {
        FN("Mesh::GetNumVertices");

        return m_vertices.size();
    }

    uint32_t GetNumTexCoords()
    {
        FN("Mesh::GetNumTexCoords");

        return m_texCoords.size();
    }

    std::vector<uint32_t> &GetIndices()
    {
        FN("Mesh::GetIndices");

        return m_indices;
    }

    std::vector<Vector3<float>> &GetVertices()
    {
        FN("Mesh::GetVertices");

        return m_vertices;
    }

    std::vector<Vector3<float>> &GetNormals()
    {
        FN("Mesh::GetNormals");

        return m_normals;
    }

    std::vector<Vector3<float>> &GetTangents()
    {
        FN("Mesh::GetTangents");

        return m_tangents;
    }

    std::vector<Vector3<float>> &GetBitangents()
    {
        FN("Mesh::GetBitangents");

        return m_bitangents;
    }

    std::vector<Vector2<float>> &GetTexCoords()
    {
        FN("Mesh::GetTexCoords");

        return m_texCoords;
    }

    void GetBoundigBox(Vector3<float> &bbox_min, Vector3<float> &bbox_max)
    {
        FN("Mesh::GetBoundigBox");

        bbox_min = m_boundingBoxMin;
        bbox_max = m_boundingBoxMax;
    }

    void RecalculateBoundingBox()
    {
        FN("Mesh::RecalculateBoundingBox");

        CalculateBoundingBox();
    }

  private:
    // you shall not copy
    Mesh(const Mesh &other) = delete;
    Mesh &operator=(const Mesh &other) = delete;

    void CalculateMissingNormals();
    void CalculateTangents();
    void CalculateBoundingBox();

    /** Loads the mesh from disk.  This call only performs IO, it
    does not parse the bytestream or check for any errors therein.
    It also does not set up submeshes, etc.  You have to call load()
    to do that.
    */
    void VPrepareImpl();

    /** Destroys data cached by prepareImpl.
     */
    void VUnprepareImpl();

    /// @copydoc Resource::VLoadImpl
    void VLoadImpl();

    /// @copydoc Resource::VPostLoadImpl
    void VPostLoadImpl();

    /// @copydoc Resource::VUnloadImpl
    void VUnloadImpl();

    char *m_dataFromDisk;

    /** A list of submeshes which make up this mesh.
    Each mesh is made up of 1 or more submeshes, which
    are each based on a single material and can have their
    own vertex data (they may not - they can share vertex data
    from the Mesh, depending on preference).
    */
    std::vector<SubMesh *> m_subMeshList;
    std::map<std::string, uint16_t> m_subMeshNameMap;

    std::vector<std::string> m_materialFilesList;

    std::vector<uint32_t> m_indices;
    std::vector<Vector3<float>> m_vertices;
    std::vector<Vector3<float>> m_normals;
    std::vector<Vector3<float>> m_tangents;
    std::vector<Vector3<float>> m_bitangents;
    std::vector<Vector2<float>> m_texCoords;

    Vector3<float> m_boundingBoxMin;
    Vector3<float> m_boundingBoxMax;
};
} // namespace bow
