#pragma once
#include <RenderDevice/BowRenderDevicePredeclares.h>
#include <RenderDevice/RenderDevice_api.h>

namespace bow
{

enum class RenderDeviceAPI : char
{
    DirectX11,
    DirectX12,
    OpenGL3x,
    Vulkan,
};

//! \brief RenderDeviceManager is a singleton and creates devices.
class RENDERDEVICE_API RenderDeviceManager
{
  public:
    ~RenderDeviceManager();

    static RenderDeviceManager &GetInstance();

    //! \brief Create an Device with an specific API
    //! \param api The API which should be used.
    //! \return shared_pointer of RenderDevice
    RenderDevicePtr CreateDevice(RenderDeviceAPI api, uint32_t deviceHandle = 0);

  protected:
    RenderDeviceManager() {}

  private:
    RenderDeviceManager(const RenderDeviceManager &) {}; //!< You shall not direct
    RenderDeviceManager &operator=(const RenderDeviceManager &) { return *this; }
};
} // namespace bow
