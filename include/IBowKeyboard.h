/**
 * @file IBowKeyboard.h
 * @brief Declarations for IBowKeyboard.
 */

#pragma once
#include "BowPrerequisites.h"
#include "BowInputPredeclares.h"

namespace bow {

	enum class Key : unsigned char
	{
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
		K_ESCAPE,
		K_MINUS,		/* - on main keyboard */
		K_EQUALS,
		K_BACK,			/* backspace */
		K_TAB,
		K_Q,
		K_W,
		K_E,
		K_R,
		K_T,
		K_Y,
		K_U,
		K_I,
		K_O,
		K_P,
		K_LBRACKET,
		K_RBRACKET,
		K_RETURN,		/* Enter on main keyboard */
		K_LCONTROL,
		K_A,
		K_S,
		K_D,
		K_F,
		K_G,
		K_H,
		K_J,
		K_K,
		K_L,
		K_SEMICOLON,
		K_APOSTROPHE,
		K_GRAVE,		/* accent grave */
		K_LSHIFT,
		K_BACKSLASH,
		K_Z,
		K_X,
		K_C,
		K_V,
		K_B,
		K_N,
		K_M,
		K_COMMA,
		K_PERIOD,		/* . on main keyboard */
		K_SLASH,		/* / on main keyboard */
		K_RSHIFT,
		K_MULTIPLY,		/* * on numeric keypad */
		K_LMENU,		/* left Alt */
		K_SPACE,
		K_CAPITAL,
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
		K_NUMLOCK,
		K_SCROLL,		/* Scroll Lock */
		K_NUMPAD7,
		K_NUMPAD8,
		K_NUMPAD9,
		K_SUBTRACT,		/* - on numeric keypad */
		K_NUMPAD4,
		K_NUMPAD5,
		K_NUMPAD6,
		K_ADD,			/* + on numeric keypad */
		K_NUMPAD1,
		K_NUMPAD2,
		K_NUMPAD3,
		K_NUMPAD0,
		K_DECIMAL,		/* . on numeric keypad */
		K_OEM_102,		/* <> or \| on RT 102-key keyboard (Non-U.S.) */
		K_F11,
		K_F12,
		K_F13,			/*                     (NEC PC98) */
		K_F14,			/*                     (NEC PC98) */
		K_F15,			/*                     (NEC PC98) */
		K_KANA,			/* (Japanese keyboard)            */
		K_ABNT_C1,		/* /? on Brazilian keyboard */
		K_CONVERT,		/* (Japanese keyboard)            */
		K_NOCONVERT,    /* (Japanese keyboard)            */
		K_YEN,			/* (Japanese keyboard)            */
		K_ABNT_C2,		/* Numpad . on Brazilian keyboard */
		K_NUMPADEQUALS,	/* = on numeric keypad (NEC PC98) */
		K_PREVTRACK,	/* Previous Track (K_CIRCUMFLEX on Japanese keyboard) */
		K_AT,			/*                     (NEC PC98) */
		K_COLON,		/*                     (NEC PC98) */
		K_UNDERLINE,	/*                     (NEC PC98) */
		K_KANJI,		/* (Japanese keyboard)            */
		K_STOP,			/*                     (NEC PC98) */
		K_AX,			/*                     (Japan AX) */
		K_UNLABELED,	/*                        (J3100) */
		K_NEXTTRACK,	/* Next Track */
		K_NUMPADENTER,	/* Enter on numeric keypad */
		K_RCONTROL,
		K_MUTE,			/* Mute */
		K_CALCULATOR,	/* Calculator */
		K_PLAYPAUSE,	/* Play / Pause */
		K_MEDIASTOP,	/* Media Stop */
		K_VOLUMEDOWN,	/* Volume - */
		K_VOLUMEUP,		/* Volume + */
		K_WEBHOME,		/* Web home */
		K_NUMPADCOMMA,	/* , on numeric keypad (NEC PC98) */
		K_DIVIDE,		/* / on numeric keypad */
		K_SYSRQ,
		K_RMENU,		/* right Alt */
		K_PAUSE,		/* Pause */
		K_HOME,			/* Home on arrow keypad */
		K_UP,			/* UpArrow on arrow keypad */
		K_PRIOR,		/* PgUp on arrow keypad */
		K_LEFT,			/* LeftArrow on arrow keypad */
		K_RIGHT,		/* RightArrow on arrow keypad */
		K_END,			/* End on arrow keypad */
		K_DOWN,			/* DownArrow on arrow keypad */
		K_NEXT,			/* PgDn on arrow keypad */
		K_INSERT,		/* Insert on arrow keypad */
		K_DELETE,		/* Delete on arrow keypad */
		K_LWIN,			/* Left Windows key */
		K_RWIN,			/* Right Windows key */
		K_APPS,			/* AppMenu key */
		K_POWER,		/* System Power */
		K_SLEEP,		/* System Sleep */
		K_WAKE,			/* System Wake */
		K_WEBSEARCH,	/* Web Search */
		K_WEBFAVORITES,	/* Web Favorites */
		K_WEBREFRESH,	/* Web Refresh */
		K_WEBSTOP,		/* Web Stop */
		K_WEBFORWARD,	/* Web Forward */
		K_WEBBACK,		/* Web Back */
		K_MYCOMPUTER,	/* My Computer */
		K_MAIL,			/* Mail */
		K_MEDIASELECT,	/* Media Select */

		/*
		*  Alternate names for keys, to facilitate transition from DOS.
		*/
		K_BACKSPACE = K_BACK,			/* backspace */
		K_NUMPADSTAR = K_MULTIPLY,		/* * on numeric keypad */
		K_LALT = K_LMENU,				/* left Alt */
		K_CAPSLOCK = K_CAPITAL,			/* CapsLock */
		K_NUMPADMINUS = K_SUBTRACT,		/* - on numeric keypad */
		K_NUMPADPLUS = K_ADD,			/* + on numeric keypad */
		K_NUMPADPERIOD = K_DECIMAL,		/* . on numeric keypad */
		K_NUMPADSLASH = K_DIVIDE,		/* / on numeric keypad */
		K_RALT = K_RMENU,				/* right Alt */
		K_UPARROW = K_UP,				/* UpArrow on arrow keypad */
		K_PGUP = K_PRIOR,				/* PgUp on arrow keypad */
		K_LEFTARROW = K_LEFT,			/* LeftArrow on arrow keypad */
		K_RIGHTARROW = K_RIGHT,			/* RightArrow on arrow keypad */
		K_DOWNARROW = K_DOWN,			/* DownArrow on arrow keypad */
		K_PGDN = K_NEXT,				/* PgDn on arrow keypad */

		/*
		*  Alternate names for keys originally not used on US keyboards.
		*/
		K_CIRCUMFLEX = K_PREVTRACK       /* Japanese keyboard */
	};

	class IKeyboard
	{
	public:
		IKeyboard(void){};
		virtual ~IKeyboard(void){}

		virtual bool VUpdate(void) = 0;
		virtual bool VIsPressed(Key keyID) const = 0;
	};

}
