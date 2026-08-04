#pragma once
#include <RenderDevice/Device/BowGraphicsWindow.h>

struct GLFWwindow;

namespace bow
{

typedef std::shared_ptr<class OGLRenderContext> OGLRenderContextPtr;
class OGLRenderDevice;

class OGLGraphicsWindow : public IGraphicsWindow
{
  public:
    OGLGraphicsWindow();
    ~OGLGraphicsWindow();

    // =========================================================================
    // INIT/RELEASE STUFF:
    // =========================================================================
    bool Initialize(uint32_t width, uint32_t height, const std::string &title, WindowType windowType, OGLRenderDevice *device);
    void VRelease();

    RenderContextPtr VGetContext() const;
    void VPollWindowEvents() const;

    void *VGetHandle() const;
    void VSetWindowTitle(const char *title) const;

    void VHideCursor() const;
    void VShowCursor() const;

    int VGetWidth() const;
    int VGetHeight() const;

    bool VIsVisible() const;
    bool VIsFocused() const;
    bool VShouldClose() const;

  private:
    // you shall not copy
    OGLGraphicsWindow(OGLGraphicsWindow &) = delete;
    OGLGraphicsWindow &operator=(const OGLGraphicsWindow &) = delete;

    static void ResizeCallback(GLFWwindow *window, int width, int height);

    OGLRenderDevice *m_ParentDevice;
    OGLRenderContextPtr m_Context;
    GLFWwindow *m_Window;

    int m_Width;
    int m_Height;
};

typedef std::shared_ptr<OGLGraphicsWindow> OGLGraphicsWindowPtr;
typedef std::unordered_map<uint32_t, OGLGraphicsWindowPtr> OGLGraphicsWindowMap;
} // namespace bow
