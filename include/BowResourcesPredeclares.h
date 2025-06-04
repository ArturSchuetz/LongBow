/**
 * @file BowResourcesPredeclares.h
 * @brief Declarations for BowResourcesPredeclares.
 */

#pragma once

namespace bow {
	typedef unsigned long long int ResourceHandle;

	class ResourceManager;
	class Resource;
		typedef std::shared_ptr<Resource> ResourcePtr;

	// Resources
	class Mesh;
		typedef std::shared_ptr<Mesh> MeshPtr;
	class MaterialCollection;
		typedef std::shared_ptr<MaterialCollection> MaterialCollectionPtr;
	class PointCloud;
		typedef std::shared_ptr<PointCloud> PointCloudPtr;
	class Image;
		typedef std::shared_ptr<Image> ImagePtr;

	// ResourceManagers
	class MeshManager;
	class PointCloudManager;
}
