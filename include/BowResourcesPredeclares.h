#pragma once

namespace bow {
	typedef unsigned long long int ResourceHandle;

	class ResourceManager;
	class Resource;
		typedef std::shared_ptr<Resource> ResourcePtr;

	// Resources
	class Mesh;
		typedef std::shared_ptr<Mesh> MeshPtr;
	class Material;
		typedef std::shared_ptr<Material> MaterialPtr;
	class PointCloud;
		typedef std::shared_ptr<PointCloud> PointCloudPtr;
	class Image;
		typedef std::shared_ptr<Image> ImagePtr;

	// ResourceManagers
	class MeshManager;
	class PointCloudManager;
}
