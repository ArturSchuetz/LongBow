#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"
#include "BowMesh.h"
#include "BowSubMesh.h"

namespace bow {
	namespace Core {

		// ---------------------------------------------------------------------------
		/** @brief A mesh represents geometry without any material.
		*/
		class ModelLoader_OBJ
		{
		public:
			struct vertex_index {
				int v_idx, vt_idx, vn_idx;
				vertex_index() : v_idx(-1), vt_idx(-1), vn_idx(-1) {}
				explicit vertex_index(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx) {}
				vertex_index(int vidx, int vtidx, int vnidx) : v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx) {}
			};

			// Index struct to support different indices for vtx/normal/texcoord.
			// -1 means not used.
			struct index_t {
				int vertex_index;
				int normal_index;
				int texcoord_index;
			};

			ModelLoader_OBJ();
			~ModelLoader_OBJ();

			/** Imports Mesh and (optionally) Material data from a .obj file.
			@remarks
				This method imports data from loaded data opened from a .obj file and places it's
				contents into the Mesh object which is passed in.
			@param inputData The Data holding the mesh.
			@param outputMesh Pointer to the Mesh object which will receive the data. Should be blank already.
			*/
			void ImportMesh(const char* inputData, Mesh* outputMesh);

		private:

			bool exportFaceGroupToShape(Mesh* mesh, const std::vector<vertex_index> &face);
			unsigned int getVertexIndex(Mesh* mesh, index_t index);
			std::istream &safeGetline(std::istream &is, std::string &t);


			// Make index zero-base, and also support relative index.
			inline bool fixIndex(int idx, int n, int *ret);
			inline std::string parseString(const char **token);
			inline int parseInt(const char **token);
			bool tryParseDouble(const char *s, const char *s_end, double *result);
			inline float parseReal(const char **token, double default_value = 0.0);
			inline void parseReal2(float *x, float *y, const char **token, const double default_x = 0.0, const double default_y = 0.0);
			inline void parseReal3(float *x, float *y, float *z, const char **token, const double default_x = 0.0, const double default_y = 0.0, const double default_z = 0.0);
			inline void parseV(float *x, float *y, float *z, float *w, const char **token, const double default_x = 0.0, const double default_y = 0.0, const double default_z = 0.0, const double default_w = 1.0);
			inline bool parseOnOff(const char **token, bool default_value = true);

			// Parse triples with index offsets : i, i / j / k, i//k, i/j
			bool parseTriple(const char **token, int vsize, int vnsize, int vtsize, vertex_index *ret);

			std::vector<Vector3<float>> vertices;
			std::vector<Vector3<float>> normals;
			std::vector<Vector2<float>> textureCoordinates;
		};
	}
}