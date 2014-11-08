#pragma once
#include "BowResourceManager.h"
#include "BowMesh.h"

#include "BowModelLoaderOBJ.h"
#include "BowMath.h"
#include "BowCore.h"

namespace Bow {
	namespace Core {

		static std::shared_ptr<ResourceManager> Instance;

		ResourceManager::ResourceManager()
		{

		}

		ResourceManager::~ResourceManager()
		{
			Release();
		}

		void ResourceManager::Release()
		{

		}

		ResourceManager& ResourceManager::GetInstance()
		{
			if (Instance.get() == nullptr)
			{
				Instance = std::shared_ptr<ResourceManager>(new ResourceManager());
			}
			return *Instance.get();
		}

		MeshPtr ResourceManager::LoadMesh(const std::string& path)
		{
			LOG_TRACE("Loading modeldata...");
			// OBJ is just for test and performance is not important
			ModelLoaderOBJ loader;
			if (loader.import(path.c_str(), false))
			{
				LOG_TRACE("Creating mesh object...");

				MeshPtr mesh(new Mesh());
				if (loader.getNumberOfIndices() > 0)
				{
					mesh->m_HasIndices = true;
					mesh->m_NumberOfIndices = loader.getNumberOfIndices();
					unsigned short max = -1;
					if (loader.getNumberOfIndices() > max)
					{
						mesh->Indices = new IndicesUnsignedInt(loader.getNumberOfIndices());
						IndicesUnsignedInt* tempIndices = (IndicesUnsignedInt*)(mesh->Indices);
						for (int i = 0; i < loader.getNumberOfIndices(); ++i)
						{
							tempIndices->Values.at(i) = (unsigned int)((loader.getIndexBuffer())[i]);
						}
					}
					else
					{
						mesh->Indices = new IndicesUnsignedShort(loader.getNumberOfIndices());
						IndicesUnsignedShort* tempIndices = (IndicesUnsignedShort*)(mesh->Indices);
						for (int i = 0; i < loader.getNumberOfIndices(); ++i)
						{
							tempIndices->Values.at(i) = (unsigned short)((loader.getIndexBuffer())[i]);
						}
					}
				}

				mesh->m_NumberOfVertices = loader.getNumberOfVertices();

				if (loader.hasPositions())
				{
					mesh->m_HasPositions = true;
					mesh->Positions = new Vector3<float>[loader.getNumberOfVertices()];
					for (int i = 0; i < loader.getNumberOfVertices(); ++i)
					{
						mesh->Positions[i] = Vector3<float>((loader.getVertexBuffer())[i].position);
					}
				}

				if (loader.hasNormals())
				{
					mesh->m_HasNormals = true;
					mesh->Normals = new Vector3<float>[loader.getNumberOfVertices()];
					for (int i = 0; i < loader.getNumberOfVertices(); ++i)
					{
						mesh->Normals[i] = Vector3<float>((loader.getVertexBuffer())[i].normal);
					}
				}

				if (loader.hasTextureCoords())
				{
					mesh->m_HasTextureCoords = true;
					mesh->TextureCoords = new Vector2<float>[loader.getNumberOfVertices()];
					for (int i = 0; i < loader.getNumberOfVertices(); ++i)
					{
						mesh->TextureCoords[i] = Vector2<float>((loader.getVertexBuffer())[i].texCoord);
					}
				}

				if (loader.hasTangents())
				{
					mesh->m_HasTangents = true;
					mesh->Tangents = new Vector4<float>[loader.getNumberOfVertices()];
					for (int i = 0; i < loader.getNumberOfVertices(); ++i)
					{
						mesh->Tangents[i] = Vector4<float>((loader.getVertexBuffer())[i].tangent);
					}
				}

				// Submeshes and Materials
				mesh->m_NumberOfSubMeshes = loader.getNumberOfMeshes();
				for (int i = 0; i < loader.getNumberOfMeshes(); ++i)
				{
					Mesh::Material Material = Mesh::Material();
					Material.ambient = loader.getMesh(i).pMaterial->ambient;
					Material.diffuse = loader.getMesh(i).pMaterial->diffuse;
					Material.specular = loader.getMesh(i).pMaterial->specular;
					Material.shininess = loader.getMesh(i).pMaterial->shininess;
					Material.alpha = loader.getMesh(i).pMaterial->alpha;

					mesh->SubMeshes.push_back(SubMesh(mesh.get(), loader.getMesh(i).startIndex, loader.getMesh(i).triangleCount, mesh->Materials.size()));
					mesh->Materials.push_back(Material);
				}

				LOG_TRACE("Loaded mesh successfully!");
				return mesh;
			}
			return nullptr;
		}
		
	}
}