#pragma once
#include "Resources/BowResourcesPredeclares.h"
#include "Resources/Resources_api.h"

#include "Resources/BowResourceManager.h"
#include "Resources/Resources/BowImage.h"

namespace bow
{

class RESOURCES_API ImageManager : public ResourceManager
{
  public:
    ~ImageManager();

    static ImageManager &GetInstance();

    void Initialize();

    /// Create a new mesh
    /// @see ResourceManager::createResource
    ImagePtr Create(const std::string &name);

    ResourcePtr CreateOrRetrieve(const std::string &name);

    ImagePtr Prepare(const std::string &filePath);

    ImagePtr Load(const std::string &filePath);

    ImagePtr CreateManual(const std::string &name);

  private:
    ImageManager(const ImageManager &) = delete;            // You shall not copy
    ImageManager &operator=(const ImageManager &) = delete; // You shall not assign
    ImageManager();

    /// @copydoc ResourceManager::createImpl
    virtual Resource *VCreateImpl(const std::string &name, ResourceHandle handle);
};

} // namespace bow
