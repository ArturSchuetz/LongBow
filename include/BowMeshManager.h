#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowResourcesPredeclares.h"

#include "BowMesh.h"

namespace Bow {
	namespace Core {

		class MeshManager : public ResourceManager
		{
		public:
			~MeshManager();

			static MeshManager& GetInstance();

			void Init(void);

			/// Create a new mesh
			/// @see ResourceManager::createResource
			MeshPtr Create(const std::string& name);

			/** Create a new mesh, or retrieve an existing one with the same
			name if it already exists.
			@param vertexBufferUsage The usage flags with which the vertex buffer(s)
			will be created
			@see ResourceManager::createOrRetrieve
			*/
			ResourcePtr CreateOrRetrieve(const std::string& name);

			/** Prepares a mesh for loading from a file.  This does the IO in advance of the call to load().
			@note
			If the model has already been created (prepared or loaded), the existing instance
			will be returned.
			@remarks
			Ogre loads model files from it's own proprietary
			format called .mesh. This is because having a single file
			format is better for runtime performance, and we also have
			control over pre-processed data (such as
			collision boxes, LOD reductions etc).
			@param filename The name of the file
			*/
			MeshPtr Prepare(const std::string& filePath);


			/** Loads a mesh from a file, making it immediately available for use.
			@note
			If the model has already been created (prepared or loaded), the existing instance
			will be returned.
			@remarks
			Ogre loads model files from it's own proprietary
			format called .mesh. This is because having a single file
			format is better for runtime performance, and we also have
			control over pre-processed data (such as
			collision boxes, LOD reductions etc).
			@param filename The name of the file
			*/
			MeshPtr Load(const std::string& filePath);

			/** Creates a new Mesh specifically for manual definition rather
			than loading from an object file.
			@remarks
			Note that once you've defined your mesh, you must call Mesh::_setBounds and
			Mesh::_setBoundingRadius in order to define the bounds of your mesh. In previous
			versions of OGRE you could call Mesh::_updateBounds, but OGRE's support of
			write-only vertex buffers makes this no longer appropriate.
			@param name The name to give the new mesh
			*/
			MeshPtr CreateManual(const std::string& name);

		private:
			MeshManager(const MeshManager&) {}; // You shall not copy
			MeshManager& operator=(const MeshManager&) { return *this; }
			MeshManager(void);

			/// @copydoc ResourceManager::createImpl
			virtual Resource* VCreateImpl(const std::string& name, ResourceHandle handle);
		};

	}
}