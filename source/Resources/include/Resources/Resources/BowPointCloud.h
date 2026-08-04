#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "CoreSystems/BowMath.h"
#include "Resources/BowResource.h"

namespace bow
{

// ---------------------------------------------------------------------------
/** @brief A mesh represents geometry without any material.
 */
class RESOURCES_API PointCloud : public Resource
{
    friend class PointCloudLoader_bin;
    friend class PointCloudLoader_xcn;
    friend class PointCloudLoader_xtc;
    friend class PointCloudLoader_xyz;

  public:
    PointCloud(ResourceManager *creator, const std::string &name, ResourceHandle handle);
    ~PointCloud();

    std::vector<Vector3<float>> GetVertices()
    {
        FN("PointCloud::GetVertices");

        return m_vertices;
    }
    std::vector<Vector3<float>> GetColors()
    {
        FN("PointCloud::GetColors");

        return m_colors;
    }
    std::vector<Vector3<float>> &GetNormals()
    {
        FN("PointCloud::GetNormals");

        return m_normals;
    }

    MeshAttribute CreateAttribute(const std::string &positionAttribute, const std::string &colorAttribute);
    MeshAttribute CreateAttribute(const std::string &positionAttribute, const std::string &normalAttribute, const std::string &colorAttribute);

    AABB<float> GetBoundingBox();

  private:
    // you shall not copy
    PointCloud(const PointCloud &other) = delete;
    PointCloud &operator=(const PointCloud &other) = delete;

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

    /// @copydoc Resource::VCalculateSize
    size_t VCalculateSize() const;

    char *m_dataFromDisk;

    std::vector<Vector3<float>> m_vertices;
    std::vector<Vector3<float>> m_colors;
    std::vector<Vector3<float>> m_normals;
};
} // namespace bow