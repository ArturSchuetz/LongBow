#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"

#include "IBowIndicesBase.h"
#include "IBowVertexAttribute.h"

#include "BowMath.h"

namespace Bow {
	namespace Core {

		// ---------------------------------------------------------------------------
		/** @brief A mesh represents a geometry or model with a single material.
		*/
		class Mesh
		{
			friend class ResourceManager;

		public:			
			struct Material
			{
				ColorRGB ambient;
				ColorRGB diffuse;
				ColorRGB specular;
				ColorRGB emissive;
				ColorRGB transparent;
				float shininess;        // [0 = min shininess, 1 = max shininess]
				float opacity;          // [0 = fully transparent, 1 = fully opaque]

				Bitmap* ambientTexture;
				Bitmap* diffuseTexture;
				Bitmap* normalsTexture;
				Bitmap* opacityTexture;

				std::string name;
			};

			IIndicesBase *m_Indices;
			std::vector<SubMesh> m_SubMeshes;
			std::vector<Material> m_Materials;

			std::vector<Vector3<float>> m_Positions;		//! Vertex Positions
			std::vector<Vector3<float>> m_Normals;			//! Vertex Normals
			std::vector<Vector2<float>> m_TextureCoords;	//! Vertex Texture Coordinates
			std::vector<Vector4<float>> m_Tangents;			//! Vertex Tangents pointing in the direction of the positive X texture axis.
			std::vector<Vector4<float>> m_Biangents;		//! Vertex Biangents pointing in the direction of the positive Y texture axis.

			Mesh();
			~Mesh();

			inline bool HasNormals() const
			{
				return !m_Normals.empty() && m_NumVertices > 0;
			}

			inline bool HasPositions() const
			{
				return !m_Positions.empty() && m_NumVertices > 0;
			}

			inline bool HasTangentsAndBitangents() const
			{
				return !m_Tangents.empty() && !m_Biangents.empty() && m_NumVertices > 0;
			}

			inline unsigned int GetNumberOfSubmeshes() const
			{
				return m_SubMeshes.size();
			}

			inline unsigned int GetNumberOfVertices() const
			{
				return m_NumVertices;
			}

			inline unsigned int GetNumberOfIndices() const
			{
				return m_NumIndices;
			}

			inline unsigned int GetNumberOfFaces() const
			{
				return m_NumFaces;
			}

			Vector3<float> m_Center;
			float m_Width;
			float m_Height;
			float m_Length;
			float m_Radius;

		private:
			Mesh(const Mesh& other) { } // you shall not copy


			unsigned int m_NumVertices; //! The number of vertices in this mesh
			unsigned int m_NumIndices; //! The number of indices in this mesh
			unsigned int m_NumFaces; //! The number of primitives (triangles, polygons, lines) in this  mesh. 
		};

	}
}