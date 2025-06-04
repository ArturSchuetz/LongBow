/**
 * @file BowPointCloud.h
 * @brief Declarations for BowPointCloud.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

#include "BowMath.h"
#include "BowResource.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A mesh represents geometry without any material.
	*/
	class PointCloud : public Resource
	{
		friend class PointCloudLoader;
	public:
		PointCloud(ResourceManager* creator, const std::string& name, ResourceHandle handle);
		~PointCloud();

		std::vector<Vector3<float>> GetVertices() { return m_vertices; }
		std::vector<Vector3<float>> GetColors() { return m_colors; }

		MeshAttribute CreateAttribute(const std::string& positionAttribute, const std::string& colorAttribute);
		MeshAttribute CreateAttribute(const std::string& positionAttribute, const std::string& normalAttribute, const std::string& colorAttribute);

		AABB<float> GetBoundingBox();

	private:
		/** Loads the mesh from disk.  This call only performs IO, it
			does not parse the bytestream or check for any errors therein.
			It also does not set up submeshes, etc.  You have to call load()
			to do that.
		*/
		void VPrepareImpl(void);

		/** Destroys data cached by prepareImpl.
		*/
		void VUnprepareImpl(void);

		/// @copydoc Resource::VLoadImpl
		void VLoadImpl(void);

		/// @copydoc Resource::VPostLoadImpl
		void VPostLoadImpl(void);

		/// @copydoc Resource::VUnloadImpl
		void VUnloadImpl(void);

		/// @copydoc Resource::VCalculateSize
		size_t VCalculateSize(void) const;

		char* m_dataFromDisk;

		std::vector<Vector3<float>> m_vertices;
		std::vector<Vector3<float>> m_colors;
	};
}