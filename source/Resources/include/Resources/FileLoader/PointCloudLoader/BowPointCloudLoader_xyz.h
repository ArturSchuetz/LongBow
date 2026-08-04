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
class PointCloudLoader_xyz
{
  public:
    PointCloudLoader_xyz();
    ~PointCloudLoader_xyz();

    /** Imports PointCloud data from a .xyz file.
    @remarks
    This method imports data from loaded data opened from a .xyz file and places
    it's contents into the PointCloud object which is passed in.
    @param inputData The Data holding the mesh.
    @param outputMesh Pointer to the PointCloud object which will receive the
    data. Should be blank already.
    */
    void ImportPointCloud(const char *inputData, PointCloud *outputMesh);

  private:
    // you shall not copy
    PointCloudLoader_xyz(const PointCloudLoader_xyz &other) = delete;
    PointCloudLoader_xyz &operator=(const PointCloudLoader_xyz &other) = delete;

    std::istream &safeGetline(std::istream &is, std::string &t);

    inline void parseReal3(float *x, float *y, float *z, const char **token, const float default_x = 0.0, const float default_y = 0.0, const float default_z = 0.0);
    inline float parseReal(const char **token, float default_value = 0.0);

    bool tryParsefloat(const char *s, const char *s_end, float *result);
};
} // namespace bow
