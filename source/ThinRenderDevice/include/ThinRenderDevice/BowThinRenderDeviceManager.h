#pragma once
#include <ThinRenderDevice/BowThinRenderDevicePredeclares.h>
#include <ThinRenderDevice/ThinRenderDevice_api.h>

namespace bow
{

//! Opens a thin backend and pulls its factory out.
/*!
    The same plugin model as RenderDeviceManager, and deliberately separate:
    a build can contain the classic backends, the thin ones, or both, and a
    program links only the manager it uses.

    Thin backends export CreateThinDevice; classic ones export
    CreateRenderDevice, so a plugin cannot be loaded by the wrong manager.
*/
class THINRENDERDEVICE_API ThinRenderDeviceManager
{
  public:
    ~ThinRenderDeviceManager();

    static ThinRenderDeviceManager &GetInstance();

    //! Loads the backend for the given API and creates a device on it.
    /*!
        \param api          Which explicit API to use.
        \param deviceHandle Adapter index, or 0 to let the API choose.
        \return The device, or null when the plugin is missing or fails.
    */
    ThinDevicePtr CreateDevice(ThinRenderDeviceAPI api, uint32_t deviceHandle = 0);

  protected:
    ThinRenderDeviceManager() {}

  private:
    ThinRenderDeviceManager(const ThinRenderDeviceManager &) = delete;
    ThinRenderDeviceManager &operator=(const ThinRenderDeviceManager &) = delete;
};

} // namespace bow
