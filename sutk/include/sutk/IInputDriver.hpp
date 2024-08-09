#pragma once

#include <sutk/defines.hpp> // for SUTK_API and Vec2Df
#include <common/Event.hpp> // for com::Event

#include <ostream> // for std::ostream

namespace SUTK
{
	enum class ModifierKeyBits : u32
	{
		ShiftBit = BIT32(0),
		CtrlBit = BIT32(1),
		AltBit = BIT32(2),
		WinBit = BIT32(3),
		CapsLockBit = BIT32(4),
		NumLockBit = BIT32(5)
	};

	struct ModifierKeys
	{
		ModifierKeyBits bits;

		constexpr ModifierKeys() noexcept : bits(com::ForceIntToEnumClass<ModifierKeyBits>(0)) { }
		constexpr ModifierKeys(ModifierKeyBits _bits) noexcept : bits(_bits) { }

		bool shift() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::ShiftBit); }
		bool ctrl() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::CtrlBit); }
		bool alt() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::AltBit); }
		bool win() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::WinBit); }
		bool capsLock() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::CapsLockBit); }
		bool numLock() const noexcept { return HAS_FLAG(bits, ModifierKeyBits::NumLockBit); }
	};

	std::ostream& operator <<(std::ostream& stream, ModifierKeys v);
	std::ostream&& operator <<(std::ostream&& stream, ModifierKeys v);

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
	public:
		typedef com::Event<IInputDriver, Vec2Df> OnMouseMoveEvent;
		typedef com::Event<IInputDriver, MouseButton, KeyEvent> OnMouseButtonEvent;
		typedef com::Event<IInputDriver, Vec2Df> OnMouseScrollEvent;
		typedef com::Event<IInputDriver, KeyCode, KeyEvent, ModifierKeys> OnKeyboardEvent;
		typedef com::Event<IInputDriver, bool> OnMouseEnterExitEvent;
	private:
		OnMouseMoveEvent m_onCursorMoveEvent;
		OnMouseButtonEvent m_onMouseButtonEvent;
		OnMouseScrollEvent m_onScrollEvent;
		OnKeyboardEvent m_onKeyEvent;
		OnMouseEnterExitEvent m_onMouseEnterExitEvent;
	public:
		virtual ~IInputDriver() = default;

		OnMouseMoveEvent& getOnCursorMoveEvent() { return m_onCursorMoveEvent; }
		OnMouseButtonEvent& getOnMouseButtonEvent() { return m_onMouseButtonEvent; }
		OnMouseScrollEvent& getOnScrollEvent() { return m_onScrollEvent; }
		OnKeyboardEvent& getOnKeyEvent() { return m_onKeyEvent; }
		OnMouseEnterExitEvent& getOnMouseEnterExitEvent() { return m_onMouseEnterExitEvent; }

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