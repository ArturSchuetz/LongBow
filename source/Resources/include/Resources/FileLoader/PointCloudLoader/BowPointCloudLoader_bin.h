#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "CoreSystems/BowMath.h"

#include "Resources/Resources/BowPointCloud.h"

namespace bow
{

// ---------------------------------------------------------------------------
/** @brief A PointCloud represents geometry without any material.
 */
class PointCloudLoader_bin
{
  public:
    PointCloudLoader_bin();
    ~PointCloudLoader_bin();

    /** Imports PointCloud data from a .xyz file.
    @remarks
    This method imports data from loaded data opened from a .xyz file and places
    it's contents into the PointCloud object which is passed in.
    @param inputData The Data holding the mesh.
    @param outputMesh Pointer to the PointCloud object which will receive the
    data. Should be blank already.
    */
    void ImportPointCloud(const char *inputData, size_t sizeInBytes, PointCloud *outputMesh);

  private:
    // you shall not copy
    PointCloudLoader_bin(const PointCloudLoader_bin &other) = delete;
    PointCloudLoader_bin &operator=(const PointCloudLoader_bin &other) = delete;
};
} // namespace bow
