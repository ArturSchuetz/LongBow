#pragma once
#include <NativeInputDevice/NativeInputDevice_api.h>

#include <InputDevice/IBowMouse.h>

#include <CoreSystems/BowMath.h>

struct GLFWwindow;

namespace bow
{

void handleScrollInput(GLFWwindow *window, double xoffset, double yoffset);

class NativeMouseDevice : public IMouse
{
    friend void handleScrollInput(GLFWwindow *window, double xoffset, double yoffset);

  public:
    NativeMouseDevice(GLFWwindow *glfwWindow);
    ~NativeMouseDevice();

    bool Initialize();
    bool VUpdate();
    bool VIsPressed(MouseButton keyID) const;

    Vector3<long> VGetRelativePosition() const;
    Vector3<long> VGetAbsolutePosition() const;
    Vector3<long> VGetAbsolutePositionInsideWindow() const;

    void VHideCursor();
    void VShowCursor();

    bool VSetCursorPosition(int x, int y);

    // Direct Input supports this with exclusive mode
    void VCageMouse(bool cage);
    void VSetCage(long left, long top, long right, long bottom);

  private:
    // you shall not copy
    NativeMouseDevice(const NativeMouseDevice &obj) = delete;
    NativeMouseDevice &operator=(const NativeMouseDevice &obj) = delete;

    double m_x;
    double m_y;
    double m_scroll;

    double m_lastX;
    double m_lastY;
    double m_lastScroll;

    float m_relativeX;
    float m_relativeY;
    float m_relativeScrollWheel;

    bool m_shouldCage;

    float m_cageBounds_left;
    float m_cageBounds_top;
    float m_cageBounds_right;
    float m_cageBounds_bottom;

    GLFWwindow *m_glfwWindow;
};

typedef std::shared_ptr<NativeMouseDevice> NativeMousePtr;

} // namespace bow
