#pragma once

namespace Bow {
	namespace Core {
		typedef unsigned long long int ResourceHandle;

		class ResourceManager;
		class Resource;
			typedef std::shared_ptr<Resource> ResourcePtr;

		// Resources
		class Mesh;
			typedef std::shared_ptr<Mesh> MeshPtr;
		class Bitmap;
		class Clipmap;

		// ResourceManagers
		class MeshManager;
	}
}
