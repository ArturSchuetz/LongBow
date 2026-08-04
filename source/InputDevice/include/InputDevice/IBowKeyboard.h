#pragma once
#include "InputDevice/BowInputPredeclares.h"
#include "InputDevice/InputDevice_api.h"

namespace bow
{

enum class Key : unsigned char
{
    /* Printable keys */
    K_SPACE,
    K_APOSTROPHE, /* ' */
    K_COMMA,      /* , */
    K_MINUS,      /* - */
    K_PERIOD,     /* . */
    K_SLASH,      /* / */
    K_0,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_SEMICOLON, /* ; */
    K_EQUAL,     /* = */
    K_A,
    K_B,
    K_C,
    K_D,
    K_E,
    K_F,
    K_G,
    K_H,
    K_I,
    K_J,
    K_K,
    K_L,
    K_M,
    K_N,
    K_O,
    K_P,
    K_Q,
    K_R,
    K_S,
    K_T,
    K_U,
    K_V,
    K_W,
    K_X,
    K_Y,
    K_Z,
    K_LEFT_BRACKET,  /* [ */
    K_BACKSLASH,     /* \ */
    K_RIGHT_BRACKET, /* ] */
    K_GRAVE_ACCENT,  /* ` */
    K_WORLD_1,       /* non-US #1 */
    K_WORLD_2,       /* non-US #2 */

    /* Function keys */
    K_ESCAPE,
    K_ENTER,
    K_TAB,
    K_BACKSPACE,
    K_INSERT,
    K_DELETE,
    K_RIGHT,
    K_LEFT,
    K_DOWN,
    K_UP,
    K_PAGE_UP,
    K_PAGE_DOWN,
    K_HOME,
    K_END,
    K_CAPS_LOCK,
    K_SCROLL_LOCK,
    K_NUM_LOCK,
    K_PRINT_SCREEN,
    K_PAUSE,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,
    K_F13,
    K_F14,
    K_F15,
    K_F16,
    K_F17,
    K_F18,
    K_F19,
    K_F20,
    K_F21,
    K_F22,
    K_F23,
    K_F24,
    K_F25,
    K_KP_0,
    K_KP_1,
    K_KP_2,
    K_KP_3,
    K_KP_4,
    K_KP_5,
    K_KP_6,
    K_KP_7,
    K_KP_8,
    K_KP_9,
    K_KP_DECIMAL,
    K_KP_DIVIDE,
    K_KP_MULTIPLY,
    K_KP_SUBTRACT,
    K_KP_ADD,
    K_KP_ENTER,
    K_KP_EQUAL,
    K_LEFT_SHIFT,
    K_LEFT_CONTROL,
    K_LEFT_ALT,
    K_LEFT_SUPER,
    K_RIGHT_SHIFT,
    K_RIGHT_CONTROL,
    K_RIGHT_ALT,
    K_RIGHT_SUPER,
    K_MENU
};

class IKeyboard
{
  public:
    IKeyboard() {};
    virtual ~IKeyboard() {}

    virtual bool VUpdate() = 0;
    virtual bool VIsPressed(Key keyID) const = 0;
};

} // namespace bow
