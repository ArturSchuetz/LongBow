#include "Resources/FileLoader/PointCloudLoader/BowPointCloudLoader_bin.h"
#include "Resources/BowResources.h"



#include <iostream>
#include <sstream>

namespace bow
{

PointCloudLoader_bin::PointCloudLoader_bin() { FN("PointCloudLoader_bin::PointCloudLoader_bin"); }

PointCloudLoader_bin::~PointCloudLoader_bin() { FN("PointCloudLoader_bin::~PointCloudLoader_bin"); }

void PointCloudLoader_bin::ImportPointCloud(const char *inputData, size_t sizeInBytes, PointCloud *outputMesh)
{
    FN("PointCloudLoader_bin::ImportPointCloud");
    OPTICK_EVENT();

    std::vector<bow::Vector4<float>> vertices;
    vertices.resize((sizeInBytes - 1) / sizeof(bow::Vector4<float>));

    memcpy(&(vertices[0]), inputData, (sizeInBytes - 1));

    for (size_t i = 0; i < vertices.size(); i++)
    {
        outputMesh->m_vertices.push_back(Vector3<float>(vertices[i].x, vertices[i].y, vertices[i].z));
    }
}
} // namespace bow
