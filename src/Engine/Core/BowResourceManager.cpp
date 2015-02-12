#pragma once
#include "BowResourceManager.h"
#include "BowMesh.h"

#include "BowModelLoaderOBJ.h"
#include "BowMath.h"
#include "BowCore.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

			if (!scene)
			{ 
				LOG_ERROR(importer.GetErrorString());
				return nullptr;
			}
			std::string folder = path.substr(0, path.find_last_of("/")+1);

			MeshPtr mesh(new Mesh());

			for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
			{
				aiMaterial *material = scene->mMaterials[i];

				Mesh::Material Material = Mesh::Material();

				aiString name;
				aiColor4D color(0.f, 0.f, 0.f, 0.0f);
				float floatvalue;

				if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
					Material.name = name.C_Str();

				if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
					Material.ambient = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
					Material.diffuse = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
					Material.specular = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
					Material.specular = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
					Material.emissive = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS)
					Material.transparent = ColorRGB(color.r, color.g, color.b);

				if (material->Get(AI_MATKEY_SHININESS, floatvalue) == AI_SUCCESS)
					Material.shininess = floatvalue;

				if (material->Get(AI_MATKEY_OPACITY, floatvalue) == AI_SUCCESS)
					Material.opacity = floatvalue;

				aiString TexturePath;
				if (material->GetTexture(aiTextureType_AMBIENT, 0, &TexturePath) == AI_SUCCESS)
					Material.ambientTexture = LoadImageFile(folder + TexturePath.C_Str());

				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &TexturePath) == AI_SUCCESS)
					Material.diffuseTexture = LoadImageFile(folder + TexturePath.C_Str());

				if (material->GetTexture(aiTextureType_OPACITY, 0, &TexturePath) == AI_SUCCESS)
					Material.opacityTexture = LoadImageFile(folder + TexturePath.C_Str());

				if (material->GetTexture(aiTextureType_NORMALS, 0, &TexturePath) == AI_SUCCESS)
					Material.normalsTexture = LoadImageFile(folder + TexturePath.C_Str());

				mesh->m_Materials.push_back(Material);
			}

			IndicesUnsignedInt *Indices = new IndicesUnsignedInt();
			mesh->m_Indices = Indices;

			float max_x = 0.0f, min_x = 0.0f,
				max_y = 0.0f, min_y = 0.0f,
				max_z = 0.0f, min_z = 0.0f;

			for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
			{
				aiMesh *submesh = scene->mMeshes[i];
				if (submesh != nullptr)
				{
					unsigned int offset = mesh->m_NumVertices;
					mesh->m_NumVertices += submesh->mNumVertices;
					for (unsigned int v = 0; v < submesh->mNumVertices; ++v)
					{
						aiVector3D pos = submesh->mVertices[v];
						mesh->m_Positions.push_back(Vector3<float>(pos.x, pos.y, pos.z));

						if (max_x < pos.x)
							max_x = pos.x;

						if (min_x > pos.x)
							min_x = pos.x;

						if (max_y < pos.y)
							max_y = pos.y;

						if (min_y > pos.y)
							min_y = pos.y;

						if (max_z < pos.z)
							max_z = pos.z;

						if (min_z > pos.z)
							min_z = pos.z;
					}

					if (submesh->HasNormals())
					{
						for (unsigned int v = 0; v < submesh->mNumVertices; ++v)
						{
							aiVector3D norm = submesh->mNormals[v];
							mesh->m_Normals.push_back(Vector3<float>(norm.x, norm.y, norm.z));
						}
					}

					if (submesh->HasTextureCoords(0))
					{
						for (unsigned int v = 0; v < submesh->mNumVertices; ++v)
						{
							aiVector3D texCoord = submesh->mTextureCoords[0][v];
							mesh->m_TextureCoords.push_back(Vector2<float>(texCoord.x, texCoord.y));
						}
					}

					SubMesh Submesh = SubMesh(mesh.get(), mesh->m_NumIndices, submesh->mNumFaces, submesh->mMaterialIndex);
					for (unsigned int f = 0; f < submesh->mNumFaces; ++f)
					{
						mesh->m_NumFaces++;

						aiFace face = submesh->mFaces[f];
						mesh->m_NumIndices += face.mNumIndices;
						Submesh.NumIndices += face.mNumIndices;

						for (unsigned int k = 0; k < face.mNumIndices; ++k)
						{
							Indices->Values.push_back(offset+face.mIndices[k]);
						}
					}

					mesh->m_SubMeshes.push_back(Submesh);
				}
			}

			mesh->m_Width = max_x - min_x;
			mesh->m_Height = max_y - min_y;
			mesh->m_Length = max_z - min_z;
			mesh->m_Center = Vector3<float>(max_x - mesh->m_Width / 2.0f, max_y - mesh->m_Height / 2.0f, max_z - mesh->m_Length / 2.0f);
			
			LOG_TRACE("Loaded mesh successfully!");
			return mesh;
		}

		Bitmap* ResourceManager::LoadImageFile(const std::string& path)
		{
			if (Images.find(path) == Images.end())
			{
				Images[path] = Bitmap();
				if (!Images[path].LoadFile(path))
				{
					return nullptr;
				}
			}
			return &Images[path];
		}
	}
}