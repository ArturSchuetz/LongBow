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
class PointCloudLoader_xcn
{
  public:
    PointCloudLoader_xcn();
    ~PointCloudLoader_xcn();

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
    PointCloudLoader_xcn(const PointCloudLoader_xcn &other) = delete;
    PointCloudLoader_xcn &operator=(const PointCloudLoader_xcn &other) = delete;

    std::istream &safeGetline(std::istream &is, std::string &t);

    inline void parseReal9(float *x, float *y, float *z, float *r, float *g, float *b, float *nx, float *ny, float *nz, const char **token, const float default_x = 0.0, const float default_y = 0.0, const float default_z = 0.0,
                           const float default_r = 1.0, const float default_g = 1.0, const float default_b = 1.0, const float default_nx = 0.0, const float default_ny = 0.0, const float default_nz = 0.0);
    inline float parseReal(const char **token, float default_value = 0.0);

    bool tryParsefloat(const char *s, const char *s_end, float *result);
};
} // namespace bow
