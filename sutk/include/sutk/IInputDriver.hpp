#pragma once

#include <sutk/defines.hpp> // for SUTK_API and Vec2Df
#include <common/Event.hpp> // for com::Event

#include <ostream> // for std::ostream

namespace SUTK
{
	enum class KeyCode
	{
		Undefined,
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
		Digit_1_Exclaim, 	// !
		Digit_2_Atsign,		// @
		Digit_3_Hash,		// #
		Digit_4_Dollar,		// $
		Digit_5_Percent,	// %
		Digit_6_Hat,		// ^
		Digit_7_Ampersand,	// &
		Digit_8_Asterisk,	// *
		Digit_9_OpenParen,  // (
		Digit_0_CloseParen, // )
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
		F25,
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

	std::ostream& operator <<(std::ostream& stream, KeyCode v);
	std::ostream&& operator <<(std::ostream&& stream, KeyCode v);

	enum class MouseButton
	{
		Left,
		Right,
		Middle,
		Backward,
		Forward
	};


	std::ostream& operator <<(std::ostream& stream, MouseButton v);
	std::ostream&& operator <<(std::ostream&& stream, MouseButton v);
	
	enum class KeyEvent
	{
		Press,
		Release
	};

	std::ostream& operator <<(std::ostream& stream, KeyEvent v);
	std::ostream&& operator <<(std::ostream&& stream, KeyEvent v);
	
	class SUTK_API IInputDriver
	{
	private:
		com::Event<IInputDriver, Vec2Df> m_onCursorMoveEvent;
		com::Event<IInputDriver, MouseButton, KeyEvent> m_onMouseButtonEvent;
		com::Event<IInputDriver, Vec2Df> m_onScrollEvent;
		com::Event<IInputDriver, KeyCode, KeyEvent> m_onKeyEvent;
	public:
		virtual ~IInputDriver() = default;

		com::Event<IInputDriver, Vec2Df>& getOnCursorMoveEvent() { return m_onCursorMoveEvent; }
		com::Event<IInputDriver, MouseButton, KeyEvent>& getOnMouseButtonEvent() { return m_onMouseButtonEvent; }
		com::Event<IInputDriver, Vec2Df>& getOnScrollEvent() { return m_onScrollEvent; }
		com::Event<IInputDriver, KeyCode, KeyEvent>& getOnKeyEvent() { return m_onKeyEvent; }

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