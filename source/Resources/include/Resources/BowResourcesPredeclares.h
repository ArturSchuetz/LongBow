#pragma once
#include "Resources/Resources_api.h"

#include <memory>

namespace bow
{

typedef unsigned long long int ResourceHandle;

// Resources
class RESOURCES_API Resource;
typedef std::shared_ptr<Resource> ResourcePtr;
class RESOURCES_API Image;
typedef std::shared_ptr<Image> ImagePtr;
class RESOURCES_API MaterialCollection;
typedef std::shared_ptr<MaterialCollection> MaterialCollectionPtr;
class RESOURCES_API Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;
class RESOURCES_API PointCloud;
typedef std::shared_ptr<PointCloud> PointCloudPtr;

// ResourceManagers
class RESOURCES_API ResourceManager;
class RESOURCES_API ImageManager;
class RESOURCES_API MaterialManager;
class RESOURCES_API MeshManager;
class RESOURCES_API PointCloudManager;

} // namespace bow