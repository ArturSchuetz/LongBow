#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"
#include "BowPlatformPredeclares.h"

#include "BowMath.h"

#include "BowMesh.h"
#include "BowMaterial.h"

namespace bow {

	// ---------------------------------------------------------------------------
	/** @brief A mesh represents geometry without any material.
	*/
	class ModelLoader_obj
	{
	public:
		struct vertex_index {
			int v_idx;
			int vt_idx;
			int vn_idx;

			vertex_index() : 
				v_idx(-1), 
				vt_idx(-1), 
				vn_idx(-1)
			{
			}

			explicit vertex_index(int idx) : 
				v_idx(idx), 
				vt_idx(idx),
				vn_idx(idx) 
			{
			}

			vertex_index(int vidx, int vtidx, int vnidx) :
				v_idx(vidx), 
				vt_idx(vtidx), 
				vn_idx(vnidx) 
			{
			}
		};

		// ==============================================
		// ==============================================

		// Index struct to support different indices for vtx/normal/texcoord.
		// -1 means not used.
		typedef struct {
			int vertex_index;
			int normal_index;
			int texcoord_index;
		} index_t;

		// ==============================================
		// ==============================================

		ModelLoader_obj();
		~ModelLoader_obj();

		/** Imports Mesh and (optionally) Material data from a .obj file.
		@remarks
			This method imports data from loaded data opened from a .obj file and places it's
			contents into the Mesh object which is passed in.
		@param inputData The Data holding the mesh.
		@param outputMesh Pointer to the Mesh object which will receive the data. Should be blank already.
		*/
		void ImportMesh(const char* inputData, Mesh* outputMesh);
		void ImportMaterial(const char* inputData, MaterialCollection* outputMesh);

	private:
		bool exportFaceGroupToShape(Mesh* mesh, const std::vector<vertex_index> &face);
		unsigned int getVertexIndex(Mesh* mesh, index_t index);
		std::istream &safeGetline(std::istream &is, std::string &t);

		// ==============================================

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
		inline bool parseTriple(const char **token, int vsize, int vnsize, int vtsize, vertex_index *ret);
		inline bool parseTextureNameAndOption(std::string *texname, const char *linebuf, const bool is_bump);

		std::vector<Vector3<float>> vertices;
		std::vector<Vector3<float>> normals;
		std::vector<Vector2<float>> texCoords;

		// material
		std::map<std::string, int> m_material_map;
		int m_material = -1;
	};
}