#pragma once
#include <DirectX11RenderDevice/BowD3D11RenderDevicePredeclares.h>
#include <DirectX11RenderDevice/DirectX11RenderDevice_api.h>

#include <RenderDevice/Device/BowGraphicsWindow.h>
#include <RenderDevice/IBowRenderDevice.h>

#include <Windows.h>

#include <string>

namespace bow
{

//! Win32 window owning one DirectX 11 swap chain.
/*!
    Unlike the OpenGL and Vulkan backends this does not go through GLFW: DXGI
    wants a plain HWND, and a raw window keeps the message loop and the swap
    chain's resize handling in one place.
*/
class D3D11GraphicsWindow : public IGraphicsWindow
{
  public:
    D3D11GraphicsWindow();
    ~D3D11GraphicsWindow();

    bool Initialize(uint32_t width, uint32_t height, const std::string &title, WindowType windowType, D3D11RenderDevice *device);

    void VRelease() override;

    RenderContextPtr VGetContext() const override;
    void VPollWindowEvents() const override;
    void VSetWindowTitle(const char *title) const override;

    void VHideCursor() const override;
    void VShowCursor() const override;

    int VGetWidth() const override;
    int VGetHeight() const override;

    bool VIsVisible() const override;
    bool VIsFocused() const override;
    bool VShouldClose() const override;

    void *VGetHandle() const override { return (void *)m_windowHandle; }

    HWND GetHandle() const { return m_windowHandle; }

  private:
    // You shall not copy
    D3D11GraphicsWindow(const D3D11GraphicsWindow &) = delete;
    D3D11GraphicsWindow &operator=(const D3D11GraphicsWindow &) = delete;

    static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

    void OnResize(uint32_t width, uint32_t height);

    HWND m_windowHandle;
    D3D11RenderDevice *m_parentDevice;
    D3D11RenderContextPtr m_context;

    uint32_t m_width;
    uint32_t m_height;

    mutable bool m_shouldClose;
    bool m_focused;
    bool m_visible;
};

} // namespace bow
