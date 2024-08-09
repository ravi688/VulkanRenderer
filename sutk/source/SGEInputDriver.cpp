#include <sutk/SGEInputDriver.hpp>
#include <sge-cpp/sge.hpp>
#include <sge-cpp/Display.hpp>
#include <GLFW/glfw3.h>

namespace SUTK
{
	static MouseButton getMouseButton(mouse_button_type_t type)
	{
		switch(type)
		{
			case MOUSE_BUTTON_TYPE_LEFT: return MouseButton::Left;
			case MOUSE_BUTTON_TYPE_RIGHT: return MouseButton::Right;
			case MOUSE_BUTTON_TYPE_MIDDLE: return MouseButton::Middle;
			default: return MouseButton::Left;
		}
	}

	static KeyEvent getKeyEventType(key_event_type_t type)
	{
		switch(type)
		{
			case KEY_EVENT_TYPE_PRESS: return KeyEvent::Press;
			case KEY_EVENT_TYPE_RELEASE: return KeyEvent::Release;
			default: return KeyEvent::Release;
		}
	}

	static ModifierKeys getModifierKeys(modifier_key_bits_t bits)
	{
		ModifierKeyBits _bits = com::ForceIntToEnumClass<ModifierKeyBits>(0);
		if(HAS_FLAG(bits, MODIFIER_KEY_SHIFT_BIT))
			_bits |= ModifierKeyBits::ShiftBit;;
		if(HAS_FLAG(bits, MODIFIER_KEY_CONTROL_BIT))
			_bits |= ModifierKeyBits::CtrlBit;
		if(HAS_FLAG(bits, MODIFIER_KEY_ALT_BIT))
			_bits |= ModifierKeyBits::AltBit;
		if(HAS_FLAG(bits, MODIFIER_KEY_WINDOWS_BIT))
			_bits |= ModifierKeyBits::WinBit;
		if(HAS_FLAG(bits, MODIFIER_KEY_CAPSLOCK_BIT))
			_bits |= ModifierKeyBits::CapsLockBit;
		if(HAS_FLAG(bits, MODIFIER_KEY_NUMLOCK_BIT))
			_bits |= ModifierKeyBits::NumLockBit;
		return { _bits };
	}

	static KeyCode getKeyCode(key_code_t keycode)
	{
		switch(keycode)
		{
			case KEY_CODE_A: return KeyCode::A;
			case KEY_CODE_B: return KeyCode::B;
			case KEY_CODE_C: return KeyCode::C;
			case KEY_CODE_D: return KeyCode::D;
			case KEY_CODE_E: return KeyCode::E;
			case KEY_CODE_F: return KeyCode::F;
			case KEY_CODE_G: return KeyCode::G;
			case KEY_CODE_H: return KeyCode::H;
			case KEY_CODE_I: return KeyCode::I;
			case KEY_CODE_J: return KeyCode::J;
			case KEY_CODE_K: return KeyCode::K;
			case KEY_CODE_L: return KeyCode::L;
			case KEY_CODE_M: return KeyCode::M;
			case KEY_CODE_N: return KeyCode::N;
			case KEY_CODE_O: return KeyCode::O;
			case KEY_CODE_P: return KeyCode::P;
			case KEY_CODE_Q: return KeyCode::Q;
			case KEY_CODE_R: return KeyCode::R;
			case KEY_CODE_S: return KeyCode::S;
			case KEY_CODE_T: return KeyCode::T;
			case KEY_CODE_U: return KeyCode::U;
			case KEY_CODE_V: return KeyCode::V;
			case KEY_CODE_W: return KeyCode::W;
			case KEY_CODE_X: return KeyCode::X;
			case KEY_CODE_Y: return KeyCode::Y;
			case KEY_CODE_Z: return KeyCode::Z;
			case KEY_CODE_1_EXCLAIM: return KeyCode::Digit_1_Exclaim;
			case KEY_CODE_2_ATSIGN: return KeyCode::Digit_2_Atsign;
			case KEY_CODE_3_HASH: return KeyCode::Digit_3_Hash;
			case KEY_CODE_4_DOLLAR: return KeyCode::Digit_4_Dollar;
			case KEY_CODE_5_PERCENT: return KeyCode::Digit_5_Percent;
			case KEY_CODE_6_HAT: return KeyCode::Digit_6_Hat;
			case KEY_CODE_7_AMPERSAND: return KeyCode::Digit_7_Ampersand;
			case KEY_CODE_8_ASTERISK: return KeyCode::Digit_8_Asterisk;
			case KEY_CODE_9_OPEN_PAREN: return KeyCode::Digit_9_OpenParen;
			case KEY_CODE_0_CLOSE_PAREN: return KeyCode::Digit_0_CloseParen;
			case KEY_CODE_RETURN_ENTER: return KeyCode::ReturnEnter;
			case KEY_CODE_ESCAPE: return KeyCode::Escape;
			case KEY_CODE_DELETE_BACKSPACE: return KeyCode::DeleteBackspace;
			case KEY_CODE_TAB: return KeyCode::Tab;
			case KEY_CODE_SPACEBAR: return KeyCode::Spacebar;
			case KEY_CODE_MINUS_UNDERSCORE: return KeyCode::MinusUnderscore;
			case KEY_CODE_EQUAL_PLUS: return KeyCode::EqualPlus;
			case KEY_CODE_OPEN_BRACKET_BRACE: return KeyCode::OpenBracketBrace;
			case KEY_CODE_CLOSE_BRACKET_BRACE: return KeyCode::CloseBracketBrace;
			case KEY_CODE_BACKSLASH_PILLOR: return KeyCode::BackslashPillor;
			case KEY_CODE_TILDE: return KeyCode::Tilde;
			case KEY_CODE_SEMICOLON_COLON: return KeyCode::SemicolonColon;
			case KEY_CODE_QUOTE_SINGLE_DOUBLE: return KeyCode::QuoteSingleDouble;
			case KEY_CODE_TILDE2: return KeyCode::Tilde2;
			case KEY_CODE_COMMA_LESS_THAN: return KeyCode::CommaLessThan;
			case KEY_CODE_FULLSTOP_GREATER_THAN: return KeyCode::FullstopGreaterThan;
			case KEY_CODE_FORWARD_SLASH_QUESTION: return KeyCode::ForwardSlashQuestion;
			case KEY_CODE_CAPSLOCK: return KeyCode::Capslock;
			case KEY_CODE_F1: return KeyCode::F1;
			case KEY_CODE_F2: return KeyCode::F2;
			case KEY_CODE_F3: return KeyCode::F3;
			case KEY_CODE_F4: return KeyCode::F4;
			case KEY_CODE_F5: return KeyCode::F5;
			case KEY_CODE_F6: return KeyCode::F6;
			case KEY_CODE_F7: return KeyCode::F7;
			case KEY_CODE_F8: return KeyCode::F8;
			case KEY_CODE_F9: return KeyCode::F9;
			case KEY_CODE_F10: return KeyCode::F10;
			case KEY_CODE_F11: return KeyCode::F11;
			case KEY_CODE_F12: return KeyCode::F12;
			case KEY_CODE_PRINTSCREEN: return KeyCode::PrintScreen;
			case KEY_CODE_SCROLLLOCK: return KeyCode::ScrollLock;
			case KEY_CODE_PAUSE: return KeyCode::Pause;
			case KEY_CODE_INSERT: return KeyCode::Insert;
			case KEY_CODE_HOME: return KeyCode::Home;
			case KEY_CODE_PAGE_UP: return KeyCode::PageUp;
			case KEY_CODE_DELETE_FORWARD: return KeyCode::DeleteForward;
			case KEY_CODE_END: return KeyCode::End;
			case KEY_CODE_PAGE_DOWN: return KeyCode::PageDown;
			case KEY_CODE_RIGHT_ARROW: return KeyCode::RightArrow;
			case KEY_CODE_LEFT_ARROW: return KeyCode::LeftArrow;
			case KEY_CODE_DOWN_ARROW: return KeyCode::DownArrow;
			case KEY_CODE_UP_ARROW: return KeyCode::UpArrow;
			case KEY_CODE_KEYPAD_NUMLOCK_CLEAR: return KeyCode::KeypadNumlockClear;
			case KEY_CODE_KEYPAD_FORWARD_SLASH: return KeyCode::KeypadForwardSlash;
			case KEY_CODE_KEYPAD_ASTERISK: return KeyCode::KeypadAsterisk;
			case KEY_CODE_KEYPAD_MINUS: return KeyCode::KeypadMinus;
			case KEY_CODE_KEYPAD_PLUS: return KeyCode::KeypadPlus;
			case KEY_CODE_KEYPAD_ENTER: return KeyCode::KeypadEnter;
			case KEY_CODE_KEYPAD_1_END: return KeyCode::Keypad1End;
			case KEY_CODE_KEYPAD_2_DOWN_ARROW: return KeyCode::Keypad2DownArrow;
			case KEY_CODE_KEYPAD_3_PAGE_DOWN: return KeyCode::Keypad3PageDown;
			case KEY_CODE_KEYPAD_4_LEFT_ARROW: return KeyCode::Keypad4LeftArrow;
			case KEY_CODE_KEYPAD_5: return KeyCode::Keypad5;
			case KEY_CODE_KEYPAD_6_RIGHT_ARROW: return KeyCode::Keypad6RightArrow;
			case KEY_CODE_KEYPAD_7_HOME: return KeyCode::Keypad7Home;
			case KEY_CODE_KEYPAD_8_UP_ARROW: return KeyCode::Keypad8UpArrow;
			case KEY_CODE_KEYPAD_9_PAGE_UP: return KeyCode::Keypad9PageUp;
			case KEY_CODE_KEYPAD_0_INSERT: return KeyCode::Keypad0Insert;
			case KEY_CODE_KEYPAD_DOT_DELETE: return KeyCode::KeypadDotDelete;
			case KEY_CODE_BACKSLASH_PILLOR_NEAR_LEFT_SHIFT: return KeyCode::BackslashPillorNearLeftShift;
			case KEY_CODE_APPLICATION: return KeyCode::Application;
			case KEY_CODE_POWER: return KeyCode::Power;
			case KEY_CODE_KEYPAD_EQUAL: return KeyCode::KeypadEqual;
			case KEY_CODE_F13: return KeyCode::F13;
			case KEY_CODE_F14: return KeyCode::F14;
			case KEY_CODE_F15: return KeyCode::F15;
			case KEY_CODE_F16: return KeyCode::F16;
			case KEY_CODE_F17: return KeyCode::F17;
			case KEY_CODE_F18: return KeyCode::F18;
			case KEY_CODE_F19: return KeyCode::F19;
			case KEY_CODE_F20: return KeyCode::F20;
			case KEY_CODE_F21: return KeyCode::F21;
			case KEY_CODE_F22: return KeyCode::F22;
			case KEY_CODE_F23: return KeyCode::F23;
			case KEY_CODE_F24: return KeyCode::F24;
			case KEY_CODE_EXECUTE: return KeyCode::Execute;
			case KEY_CODE_HELP: return KeyCode::Help;
			case KEY_CODE_MENU: return KeyCode::Menu;
			case KEY_CODE_SELECT: return KeyCode::Select;
			case KEY_CODE_STOP: return KeyCode::Stop;
			case KEY_CODE_AGAIN: return KeyCode::Again;
			case KEY_CODE_UNDO: return KeyCode::Undo;
			case KEY_CODE_CUT: return KeyCode::Cut;
			case KEY_CODE_COPY: return KeyCode::Copy;
			case KEY_CODE_PASTE: return KeyCode::Paste;
			case KEY_CODE_FIND: return KeyCode::Find;
			case KEY_CODE_MUTE: return KeyCode::Mute;
			case KEY_CODE_VOLUME_UP: return KeyCode::VolumeUp;
			case KEY_CODE_VOLUME_DOWN: return KeyCode::VolumeDown;
			case KEY_CODE_NUMLOCK: return KeyCode::Numlock;
			case KEY_CODE_KEYPAD_COMMA: return KeyCode::KeypadComma;
			case KEY_CODE_KEYPAD_EQUAL_SIGN: return KeyCode::KeypadEqualSign;
			case KEY_CODE_LEFT_CONTROL: return KeyCode::LeftControl;
			case KEY_CODE_LEFT_SHIFT: return KeyCode::LeftShift;
			case KEY_CODE_LEFT_ALT: return KeyCode::LeftAlt;
			case KEY_CODE_LEFT_GUI: return KeyCode::LeftGui;
			case KEY_CODE_RIGHT_CONTROL: return KeyCode::RightControl;
			case KEY_CODE_RIGHT_SHIFT: return KeyCode::RightShift;
			case KEY_CODE_RIGHT_ALT: return KeyCode::RightAlt;
			case KEY_CODE_RIGHT_GUI: return KeyCode::RightGui;
			default:
				debug_log_error("Key unrecognized in SUTK");
				return KeyCode::Undefined;
		}
	}

	SGEInputDriver::SGEInputDriver(SGE::Driver& driver) noexcept : m_window(driver.getRenderWindow())
	{
		getOnCursorMoveEvent().setPublisher(this);
		m_window.getOnCursorMoveEvent().subscribe([](void* publisher, void* handlerData)
		{ 
			auto* inputDriver = reinterpret_cast<SGEInputDriver*>(handlerData);
			auto window = inputDriver->m_window;
			vec2_t pos = window.getCursorPosition();
			extent2d_t inches = SGE::Display::ConvertPixelsToInches({ pos.x, pos.y });
			inputDriver->getOnCursorMoveEvent().publish({ inches.x * CENTIMETERS_PER_INCH, inches.y * CENTIMETERS_PER_INCH });
		}, reinterpret_cast<void*>(this));

		getOnMouseButtonEvent().setPublisher(this);
		m_window.getOnMouseButtonEvent().subscribe([](void* publisher, void* handlerData)
		{
			auto* inputDriver = reinterpret_cast<SGEInputDriver*>(handlerData);
			auto window = inputDriver->m_window;
			auto mouseButtonType = window.getMouseButtonType();
			auto keyEventType = window.getKeyEventType();
			inputDriver->getOnMouseButtonEvent().publish(SUTK::getMouseButton(mouseButtonType), SUTK::getKeyEventType(keyEventType));
		}, reinterpret_cast<void*>(this));

		getOnScrollEvent().setPublisher(this);
		m_window.getOnScrollEvent().subscribe([](void* publisher, void* handlerData)
		{
			auto* inputDriver = reinterpret_cast<SGEInputDriver*>(handlerData);
			auto window = inputDriver->m_window;
			auto delta = window.getScrollDelta();
			inputDriver->getOnScrollEvent().publish({ delta.x, delta.y });
		}, reinterpret_cast<void*>(this));

		getOnKeyEvent().setPublisher(this);
		m_window.getOnKeyEvent().subscribe([](void* publisherData, void* handlerData)
		{
			auto* inputDriver = reinterpret_cast<SGEInputDriver*>(handlerData);
			auto data = reinterpret_cast<event_publisher_arg_data_t*>(publisherData);
			auto keyData = reinterpret_cast<key_event_data_t*>(data->arg);
			inputDriver->getOnKeyEvent().publish(getKeyCode(keyData->keycode), SUTK::getKeyEventType(keyData->event_type), getModifierKeys(keyData->modifiers));
		}, reinterpret_cast<void*>(this));
	}

	// Keyboard
	bool SGEInputDriver::getKey(KeyCode keycode) noexcept
	{
		return false;
	}

	bool SGEInputDriver::getKeyDown(KeyCode keycode) noexcept
	{
		return false;
	}

	bool SGEInputDriver::getKeyUp(KeyCode keycode) noexcept
	{
		return false;
	}

	// Mouse
	Vec2Df SGEInputDriver::getMousePosition() noexcept
	{
		vec2_t pos = m_window.getCursorPosition();
		extent2d_t inches = SGE::Display::ConvertPixelsToInches({ pos.x, pos.y });
		return { inches.x * CENTIMETERS_PER_INCH, inches.y * CENTIMETERS_PER_INCH };
	}

	Vec2Df SGEInputDriver::getMouseScrollDelta() noexcept
	{
		return { };
	}

	bool SGEInputDriver::getMouseButton(MouseButton button) noexcept
	{
		return false;
	}

	bool SGEInputDriver::getMouseButtonDown(MouseButton button) noexcept
	{
		return false;
	}

	bool SGEInputDriver::getMouseButtonUp(MouseButton button) noexcept
	{
		return false;
	}

}