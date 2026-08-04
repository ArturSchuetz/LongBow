#pragma once
#include <NativeInputDevice/NativeInputDevice_api.h>

#include <InputDevice/IBowKeyboard.h>

struct GLFWwindow;

namespace bow
{

class NativeKeyboardDevice : public IKeyboard
{
  public:
    NativeKeyboardDevice(GLFWwindow *glfwWindow);
    ~NativeKeyboardDevice();

    bool Initialize();
    bool VUpdate();
    bool VIsPressed(Key keyID) const;

  protected:
    char m_keys[256];

    GLFWwindow *m_glfwWindow;

  private:
    // you shall not copy
    NativeKeyboardDevice(const NativeKeyboardDevice &obj) = delete;
    NativeKeyboardDevice &operator=(const NativeKeyboardDevice &obj) = delete;
};

typedef std::shared_ptr<NativeKeyboardDevice> NativeKeyboardPtr;
} // namespace bow
