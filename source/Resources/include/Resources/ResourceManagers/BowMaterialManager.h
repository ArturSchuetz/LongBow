#pragma once
#include "CoreSystems/BowCorePredeclares.h"
#include "Resources/BowResourcesPredeclares.h"

#include "Resources/BowResourceManager.h"
#include "Resources/Resources/BowMaterial.h"

namespace bow
{

class RESOURCES_API MaterialManager : public ResourceManager
{
  public:
    ~MaterialManager();

    static MaterialManager &GetInstance();

    void Initialize();

    /// Create a new mesh
    /// @see ResourceManager::createResource
    MaterialCollectionPtr Create(const std::string &name);

    /** Create a new material, or retrieve an existing one with the same
    name if it already exists.
    @param vertexBufferUsage The usage flags with which the vertex buffer(s)
    will be created
    @see ResourceManager::createOrRetrieve
    */
    ResourcePtr CreateOrRetrieve(const std::string &name);

    /** Prepares a material for loading from a file.  This does the IO in
    advance of the call to load().
    @note
    If the model has already been created (prepared or loaded), the existing
    instance will be returned.
    @remarks
    Ogre loads model files from it's own proprietary
    format called .mesh. This is because having a single file
    format is better for runtime performance, and we also have
    control over pre-processed data (such as
    collision boxes, LOD reductions etc).
    @param filename The name of the file
    */
    MaterialCollectionPtr Prepare(const std::string &filePath);

    /** Loads a material from a file, making it immediately available for use.
    @note
    If the model has already been created (prepared or loaded), the existing
    instance will be returned.
    @remarks
    Ogre loads model files from it's own proprietary
    format called .mesh. This is because having a single file
    format is better for runtime performance, and we also have
    control over pre-processed data (such as
    collision boxes, LOD reductions etc).
    @param filename The name of the file
    */
    MaterialCollectionPtr Load(const std::string &filePath);

    /** Creates a new material specifically for manual definition rather
    than loading from an object file.
    @remarks
    Note that once you've defined your mesh, you must call '::_setBounds and
    Mesh::_setBoundingRadius in order to define the bounds of your mesh. In
    previous versions of OGRE you could call Mesh::_updateBounds, but OGRE's
    support of write-only vertex buffers makes this no longer appropriate.
    @param name The name to give the new mesh
    */
    MaterialCollectionPtr CreateManual(const std::string &name);

  private:
    MaterialManager(const MaterialManager &) = delete;            // You shall not copy
    MaterialManager &operator=(const MaterialManager &) = delete; // You shall not assign
    MaterialManager();

    /// @copydoc ResourceManager::createImpl
    virtual Resource *VCreateImpl(const std::string &name, ResourceHandle handle);
};

} // namespace bow
