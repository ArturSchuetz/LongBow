#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"
#include "BowPointCloud.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A PointCloud represents geometry without any material.
	*/
	class PointCloudLoader
	{
	public:
		PointCloudLoader();
		~PointCloudLoader();

		/** Imports PointCloud data from a .xyz file.
		@remarks
			This method imports data from loaded data opened from a .xyz file and places it's
			contents into the PointCloud object which is passed in.
		@param inputData The Data holding the mesh.
		@param outputMesh Pointer to the PointCloud object which will receive the data. Should be blank already.
		*/
		void ImportPointCloud(const char* inputData, PointCloud* outputMesh);

	private:
		std::istream &safeGetline(std::istream &is, std::string &t);

		inline void parseReal3(float *x, float *y, float *z, const char **token, const double default_x = 0.0, const double default_y = 0.0, const double default_z = 0.0);
		inline float parseReal(const char **token, double default_value = 0.0);

		bool tryParseDouble(const char *s, const char *s_end, double *result);

		std::vector<Vector3<float>> m_vertices;
	};
}
