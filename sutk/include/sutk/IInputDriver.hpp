#pragma once

#include <sutk/defines.hpp> // for SUTK_API and Vec2Df
#include <common/Event.hpp> // for com::Event

namespace SUTK
{
	enum class KeyCode
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Exclaim, 	// !
		Atsign,		// @
		Hash,		// #
		Dollar,		// $
		Percent,	// %
		Hat,		// ^
		Ampersand,	// &
		Asterisk,	// *
		OpenParen,  // (
		CloseParen, // )
		ReturnEnter, // <Enter>
		Escape, // <Esc>
		DeleteBackspace,
		Tab,
		Spacebar,
		MinusUnderscore,
		EqualPlus,
		OpenBracketBrace,
		CloseBracketBrace,
		BackslashPillor,
		Tilde,
		SemicolonColon,
		QuoteSingleDouble,
		Tilde2,
		CommaLessThan,
		FullstopGreaterThan,
		ForwardSlashQuestion,
		Capslock,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		PrintScreen,
		ScrollLock,
		Pause,
		Insert,
		Home,
		PageUp,
		DeleteForward,
		End,
		PageDown,
		RightArrow,
		LeftArrow,
		DownArrow,
		UpArrow,
		KeypadNumlockClear,
		KeypadForwardSlash,
		KeypadAsterisk,
		KeypadMinus,
		KeypadPlus,
		KeypadEnter,
		Keypad1End,
		Keypad2DownArrow,
		Keypad3PageDown,
		Keypad4LeftArrow,
		Keypad5,
		Keypad6RightArrow,
		Keypad7Home,
		Keypad8UpArrow,
		Keypad9PageUp,
		Keypad0Insert,
		KeypadDotDelete,
		BackslashPillorNearLeftShift,
		Application,
		WindowsKeyWin95 = Application,
		Power,
		KeypadEqual,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		Execute,
		Help,
		Menu,
		Select,
		Stop,
		Again,
		Undo,
		Cut,
		Copy,
		Paste,
		Find,
		Mute,
		VolumeUp,
		VolumeDown,
		Numlock,
		KeypadComma,
		KeypadEqualSign,
		LeftControl,
		LeftShift,
		LeftAlt,
		LeftGui,
		RightControl,
		RightShift,
		RightAlt,
		RightGui
	};

	enum class MouseButton
	{
		Left,
		Right,
		Middle,
		Backward,
		Forward
	};

	class SUTK_API IInputDriver
	{
	private:
		com::Event<IInputDriver, Vec2Df> m_onCursorMoveEvent;
		com::Event<IInputDriver> m_onKeyEvent;
	public:
		virtual ~IInputDriver() = default;

		com::Event<IInputDriver, Vec2Df>& getOnCursorMoveEvent() { return m_onCursorMoveEvent; }
		com::Event<IInputDriver>& getOnKeyEvent() { return m_onKeyEvent; }

		// Keyboard
		virtual bool getKey(KeyCode keycode) noexcept = 0;
		virtual bool getKeyDown(KeyCode keycode) noexcept = 0;
		virtual bool getKeyUp(KeyCode keycode) noexcept = 0;

		// Mouse
		virtual Vec2Df getMousePosition() noexcept = 0;
		virtual Vec2Df getMouseScrollDelta() noexcept = 0;
		virtual bool getMouseButton(MouseButton button) noexcept = 0;
		virtual bool getMouseButtonDown(MouseButton button) noexcept = 0;
		virtual bool getMouseButtonUp(MouseButton button) noexcept = 0;
	};
}