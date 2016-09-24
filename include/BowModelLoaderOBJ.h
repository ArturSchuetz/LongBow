#pragma once
#include "BowPrerequisites.h"
#include "BowResourcesPredeclares.h"

namespace Bow {
	namespace Core {

		class ModelLoaderOBJ
		{
		public:
			struct Material
			{
				float ambient[4];
				float diffuse[4];
				float specular[4];
				float shininess;        // [0 = min shininess, 1 = max shininess]
				float alpha;            // [0 = fully transparent, 1 = fully opaque]

				std::string name;
				std::string colorMapFilename;
				std::string bumpMapFilename;
			};

			struct Vertex
			{
				float position[3];
				float normal[3];
				float texCoord[2];
				float tangent[4];
				float bitangent[3];
			};

			struct Mesh
			{
				int startIndex;
				int triangleCount;
				const Material *pMaterial;
			};

			ModelLoaderOBJ();
			~ModelLoaderOBJ();

			void destroy();
			bool import(const char *pszFilename, bool rebuildNormals = false);
			void normalize(float scaleTo = 1.0f, bool center = true);
			void reverseWinding();

			// Getter methods.

			void getCenter(float &x, float &y, float &z) const;
			float getWidth() const;
			float getHeight() const;
			float getLength() const;
			float getRadius() const;

			const int *getIndexBuffer() const;
			int getIndexSize() const;

			const Material &getMaterial(int i) const;
			const Mesh &getMesh(int i) const;

			int getNumberOfIndices() const;
			int getNumberOfMaterials() const;
			int getNumberOfMeshes() const;
			int getNumberOfTriangles() const;
			int getNumberOfVertices() const;

			const std::string &getPath() const;

			const Vertex &getVertex(int i) const;
			const Vertex *getVertexBuffer() const;
			int getVertexSize() const;

			bool hasNormals() const;
			bool hasPositions() const;
			bool hasTangents() const;
			bool hasTextureCoords() const;

		private:
			void addTrianglePos(int index, int material,
				int v0, int v1, int v2);
			void addTrianglePosNormal(int index, int material,
				int v0, int v1, int v2,
				int vn0, int vn1, int vn2);
			void addTrianglePosTexCoord(int index, int material,
				int v0, int v1, int v2,
				int vt0, int vt1, int vt2);
			void addTrianglePosTexCoordNormal(int index, int material,
				int v0, int v1, int v2,
				int vt0, int vt1, int vt2,
				int vn0, int vn1, int vn2);
			int addVertex(int hash, const Vertex *pVertex);
			void bounds(float center[3], float &width, float &height,
				float &length, float &radius) const;
			void buildMeshes();
			void generateNormals();
			void generateTangents();
			void importGeometryFirstPass(FILE *pFile);
			void importGeometrySecondPass(FILE *pFile);
			bool importMaterials(const char *pszFilename);
			void scale(float scaleFactor, float offset[3]);

			bool m_hasPositions;
			bool m_hasTextureCoords;
			bool m_hasNormals;
			bool m_hasTangents;

			int m_numberOfVertexCoords;
			int m_numberOfTextureCoords;
			int m_numberOfNormals;
			int m_numberOfTriangles;
			int m_numberOfMaterials;
			int m_numberOfMeshes;

			float m_center[3];
			float m_width;
			float m_height;
			float m_length;
			float m_radius;

			std::string m_directoryPath;

			std::vector<Mesh> m_meshes;
			std::vector<Material> m_materials;
			std::vector<Vertex> m_vertexBuffer;
			std::vector<int> m_indexBuffer;
			std::vector<int> m_attributeBuffer;
			std::vector<float> m_vertexCoords;
			std::vector<float> m_textureCoords;
			std::vector<float> m_normals;

			std::map<std::string, int> m_materialCache;
			std::map<int, std::vector<int> > m_vertexCache;
		};

		//-----------------------------------------------------------------------------

		inline void ModelLoaderOBJ::getCenter(float &x, float &y, float &z) const
		{
			x = m_center[0]; y = m_center[1]; z = m_center[2];
		}

		inline float ModelLoaderOBJ::getWidth() const
		{
			return m_width;
		}

		inline float ModelLoaderOBJ::getHeight() const
		{
			return m_height;
		}

		inline float ModelLoaderOBJ::getLength() const
		{
			return m_length;
		}

		inline float ModelLoaderOBJ::getRadius() const
		{
			return m_radius;
		}

		inline const int *ModelLoaderOBJ::getIndexBuffer() const
		{
			return &m_indexBuffer[0];
		}

		inline int ModelLoaderOBJ::getIndexSize() const
		{
			return static_cast<int>(sizeof(int));
		}

		inline const ModelLoaderOBJ::Material &ModelLoaderOBJ::getMaterial(int i) const
		{
			return m_materials[i];
		}

		inline const ModelLoaderOBJ::Mesh &ModelLoaderOBJ::getMesh(int i) const
		{
			return m_meshes[i];
		}

		inline int ModelLoaderOBJ::getNumberOfIndices() const
		{
			return m_numberOfTriangles * 3;
		}

		inline int ModelLoaderOBJ::getNumberOfMaterials() const
		{
			return m_numberOfMaterials;
		}

		inline int ModelLoaderOBJ::getNumberOfMeshes() const
		{
			return m_numberOfMeshes;
		}

		inline int ModelLoaderOBJ::getNumberOfTriangles() const
		{
			return m_numberOfTriangles;
		}

		inline int ModelLoaderOBJ::getNumberOfVertices() const
		{
			return static_cast<int>(m_vertexBuffer.size());
		}

		inline const std::string &ModelLoaderOBJ::getPath() const
		{
			return m_directoryPath;
		}

		inline const ModelLoaderOBJ::Vertex &ModelLoaderOBJ::getVertex(int i) const
		{
			return m_vertexBuffer[i];
		}

		inline const ModelLoaderOBJ::Vertex *ModelLoaderOBJ::getVertexBuffer() const
		{
			return &m_vertexBuffer[0];
		}

		inline int ModelLoaderOBJ::getVertexSize() const
		{
			return static_cast<int>(sizeof(Vertex));
		}

		inline bool ModelLoaderOBJ::hasNormals() const
		{
			return m_hasNormals;
		}

		inline bool ModelLoaderOBJ::hasPositions() const
		{
			return m_hasPositions;
		}

		inline bool ModelLoaderOBJ::hasTangents() const
		{
			return m_hasTangents;
		}

		inline bool ModelLoaderOBJ::hasTextureCoords() const
		{
			return m_hasTextureCoords;
		}

	}
}