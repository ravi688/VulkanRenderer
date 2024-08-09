#include <sutk/IInputDriver.hpp>

namespace SUTK
{

	std::ostream&& operator <<(std::ostream&& stream, ModifierKeys v)
	{
		auto& _stream = operator <<(stream, v);
		return std::move(_stream);		
	}

	std::ostream& operator <<(std::ostream& stream, ModifierKeys v)
	{
		bool isPrev = false;
		if(v.shift())
		{
			stream << "Shift";
			isPrev = true;
		}
		if(v.ctrl())
		{
			if(isPrev)
				stream << " | ";
			stream <<  "Ctrl";
			isPrev = true;
		}
		if(v.alt())
		{
			if(isPrev)
				stream << " | ";
			stream <<  "Atl";
			isPrev = true;
		}
		if(v.win())
		{
			if(isPrev)
				stream << " | ";
			stream <<  "Win";
			isPrev = true;
		}
		if(v.capsLock())
		{
			if(isPrev)
				stream << " | ";
			stream <<  "CapsLock";
			isPrev = true;
		}
		if(v.numLock())
		{
			if(isPrev)
				stream << " | ";
			stream <<  "NumLock";
			isPrev = true;
		}
		if(!isPrev)
			stream << "<No Modifiers>";
		return stream;
	}

	std::ostream&& operator <<(std::ostream&& stream, KeyCode v)
	{
		auto& _stream = operator <<(stream, v);
		return std::move(_stream);
	}

	std::ostream& operator <<(std::ostream& stream, KeyCode v)
	{
		switch(v)
		{
			case KeyCode::Undefined: stream << "KeyCode::Undefined"; break;
			case KeyCode::A: stream << "KeyCode::A"; break;
			case KeyCode::B: stream << "KeyCode::B"; break;
			case KeyCode::C: stream << "KeyCode::C"; break;
			case KeyCode::D: stream << "KeyCode::D"; break;
			case KeyCode::E: stream << "KeyCode::E"; break;
			case KeyCode::F: stream << "KeyCode::F"; break;
			case KeyCode::G: stream << "KeyCode::G"; break;
			case KeyCode::H: stream << "KeyCode::H"; break;
			case KeyCode::I: stream << "KeyCode::I"; break;
			case KeyCode::J: stream << "KeyCode::J"; break;
			case KeyCode::K: stream << "KeyCode::K"; break;
			case KeyCode::L: stream << "KeyCode::L"; break;
			case KeyCode::M: stream << "KeyCode::M"; break;
			case KeyCode::N: stream << "KeyCode::N"; break;
			case KeyCode::O: stream << "KeyCode::O"; break;
			case KeyCode::P: stream << "KeyCode::P"; break;
			case KeyCode::Q: stream << "KeyCode::Q"; break;
			case KeyCode::R: stream << "KeyCode::R"; break;
			case KeyCode::S: stream << "KeyCode::S"; break;
			case KeyCode::T: stream << "KeyCode::T"; break;
			case KeyCode::U: stream << "KeyCode::U"; break;
			case KeyCode::V: stream << "KeyCode::V"; break;
			case KeyCode::W: stream << "KeyCode::W"; break;
			case KeyCode::X: stream << "KeyCode::X"; break;
			case KeyCode::Y: stream << "KeyCode::Y"; break;
			case KeyCode::Z: stream << "KeyCode::Z"; break;
			case KeyCode::Digit_1_Exclaim: stream << "KeyCode::Digit_1_Exclaim"; break;
			case KeyCode::Digit_2_Atsign: stream << "KeyCode::Digit_2_Atsign"; break;
			case KeyCode::Digit_3_Hash: stream << "KeyCode::Digit_3_Hash"; break;
			case KeyCode::Digit_4_Dollar: stream << "KeyCode::Digit_4_Dollar"; break;
			case KeyCode::Digit_5_Percent: stream << "KeyCode::Digit_5_Percent"; break;
			case KeyCode::Digit_6_Hat: stream << "KeyCode::Digit_6_Hat"; break;
			case KeyCode::Digit_7_Ampersand: stream << "KeyCode::Digit_7_Ampersand"; break;
			case KeyCode::Digit_8_Asterisk: stream << "KeyCode::Digit_8_Asterisk"; break;
			case KeyCode::Digit_9_OpenParen: stream << "KeyCode::Digit_9_OpenParen"; break;
			case KeyCode::Digit_0_CloseParen: stream << "KeyCode::Digit_0_CloseParen"; break;
			case KeyCode::ReturnEnter: stream << "KeyCode::ReturnEnter"; break;
			case KeyCode::Escape: stream << "KeyCode::Escape"; break;
			case KeyCode::DeleteBackspace: stream << "KeyCode::DeleteBackspace"; break;
			case KeyCode::Tab: stream << "KeyCode::Tab"; break;
			case KeyCode::Spacebar: stream << "KeyCode::Spacebar"; break;
			case KeyCode::MinusUnderscore: stream << "KeyCode::MinusUnderscore"; break;
			case KeyCode::EqualPlus: stream << "KeyCode::EqualPlus"; break;
			case KeyCode::OpenBracketBrace: stream << "KeyCode::OpenBracketBrace"; break;
			case KeyCode::CloseBracketBrace: stream << "KeyCode::CloseBracketBrace"; break;
			case KeyCode::BackslashPillor: stream << "KeyCode::BackslashPillor"; break;
			case KeyCode::Tilde: stream << "KeyCode::Tilde"; break;
			case KeyCode::SemicolonColon: stream << "KeyCode::SemicolonColon"; break;
			case KeyCode::QuoteSingleDouble: stream << "KeyCode::QuoteSingleDouble"; break;
			case KeyCode::Tilde2: stream << "KeyCode::Tilde2"; break;
			case KeyCode::CommaLessThan: stream << "KeyCode::CommaLessThan"; break;
			case KeyCode::FullstopGreaterThan: stream << "KeyCode::FullstopGreaterThan"; break;
			case KeyCode::ForwardSlashQuestion: stream << "KeyCode::ForwardSlashQuestion"; break;
			case KeyCode::Capslock: stream << "KeyCode::Capslock"; break;
			case KeyCode::F1: stream << "KeyCode::F1"; break;
			case KeyCode::F2: stream << "KeyCode::F2"; break;
			case KeyCode::F3: stream << "KeyCode::F3"; break;
			case KeyCode::F4: stream << "KeyCode::F4"; break;
			case KeyCode::F5: stream << "KeyCode::F5"; break;
			case KeyCode::F6: stream << "KeyCode::F6"; break;
			case KeyCode::F7: stream << "KeyCode::F7"; break;
			case KeyCode::F8: stream << "KeyCode::F8"; break;
			case KeyCode::F9: stream << "KeyCode::F9"; break;
			case KeyCode::F10: stream << "KeyCode::F10"; break;
			case KeyCode::F11: stream << "KeyCode::F11"; break;
			case KeyCode::F12: stream << "KeyCode::F12"; break;
			case KeyCode::PrintScreen: stream << "KeyCode::PrintScreen"; break;
			case KeyCode::ScrollLock: stream << "KeyCode::ScrollLock"; break;
			case KeyCode::Pause: stream << "KeyCode::Pause"; break;
			case KeyCode::Insert: stream << "KeyCode::Insert"; break;
			case KeyCode::Home: stream << "KeyCode::Home"; break;
			case KeyCode::PageUp: stream << "KeyCode::PageUp"; break;
			case KeyCode::DeleteForward: stream << "KeyCode::DeleteForward"; break;
			case KeyCode::End: stream << "KeyCode::End"; break;
			case KeyCode::PageDown: stream << "KeyCode::PageDown"; break;
			case KeyCode::RightArrow: stream << "KeyCode::RightArrow"; break;
			case KeyCode::LeftArrow: stream << "KeyCode::LeftArrow"; break;
			case KeyCode::DownArrow: stream << "KeyCode::DownArrow"; break;
			case KeyCode::UpArrow: stream << "KeyCode::UpArrow"; break;
			case KeyCode::KeypadNumlockClear: stream << "KeyCode::KeypadNumlockClear"; break;
			case KeyCode::KeypadForwardSlash: stream << "KeyCode::KeypadForwardSlash"; break;
			case KeyCode::KeypadAsterisk: stream << "KeyCode::KeypadAsterisk"; break;
			case KeyCode::KeypadMinus: stream << "KeyCode::KeypadMinus"; break;
			case KeyCode::KeypadPlus: stream << "KeyCode::KeypadPlus"; break;
			case KeyCode::KeypadEnter: stream << "KeyCode::KeypadEnter"; break;
			case KeyCode::Keypad1End: stream << "KeyCode::Keypad1End"; break;
			case KeyCode::Keypad2DownArrow: stream << "KeyCode::Keypad2DownArrow"; break;
			case KeyCode::Keypad3PageDown: stream << "KeyCode::Keypad3PageDown"; break;
			case KeyCode::Keypad4LeftArrow: stream << "KeyCode::Keypad4LeftArrow"; break;
			case KeyCode::Keypad5: stream << "KeyCode::Keypad5"; break;
			case KeyCode::Keypad6RightArrow: stream << "KeyCode::Keypad6RightArrow"; break;
			case KeyCode::Keypad7Home: stream << "KeyCode::Keypad7Home"; break;
			case KeyCode::Keypad8UpArrow: stream << "KeyCode::Keypad8UpArrow"; break;
			case KeyCode::Keypad9PageUp: stream << "KeyCode::Keypad9PageUp"; break;
			case KeyCode::Keypad0Insert: stream << "KeyCode::Keypad0Insert"; break;
			case KeyCode::KeypadDotDelete: stream << "KeyCode::KeypadDotDelete"; break;
			case KeyCode::BackslashPillorNearLeftShift: stream << "KeyCode::BackslashPillorNearLeftShift"; break;
			case KeyCode::Application: stream << "KeyCode::Application"; break;
			case KeyCode::Power: stream << "KeyCode::Power"; break;
			case KeyCode::KeypadEqual: stream << "KeyCode::KeypadEqual"; break;
			case KeyCode::F13: stream << "KeyCode::F13"; break;
			case KeyCode::F14: stream << "KeyCode::F14"; break;
			case KeyCode::F15: stream << "KeyCode::F15"; break;
			case KeyCode::F16: stream << "KeyCode::F16"; break;
			case KeyCode::F17: stream << "KeyCode::F17"; break;
			case KeyCode::F18: stream << "KeyCode::F18"; break;
			case KeyCode::F19: stream << "KeyCode::F19"; break;
			case KeyCode::F20: stream << "KeyCode::F20"; break;
			case KeyCode::F21: stream << "KeyCode::F21"; break;
			case KeyCode::F22: stream << "KeyCode::F22"; break;
			case KeyCode::F23: stream << "KeyCode::F23"; break;
			case KeyCode::F24: stream << "KeyCode::F24"; break;
			case KeyCode::F25: stream << "KeyCode::F25"; break;
			case KeyCode::Execute: stream << "KeyCode::Execute"; break;
			case KeyCode::Help: stream << "KeyCode::Help"; break;
			case KeyCode::Menu: stream << "KeyCode::Menu"; break;
			case KeyCode::Select: stream << "KeyCode::Select"; break;
			case KeyCode::Stop: stream << "KeyCode::Stop"; break;
			case KeyCode::Again: stream << "KeyCode::Again"; break;
			case KeyCode::Undo: stream << "KeyCode::Undo"; break;
			case KeyCode::Cut: stream << "KeyCode::Cut"; break;
			case KeyCode::Copy: stream << "KeyCode::Copy"; break;
			case KeyCode::Paste: stream << "KeyCode::Paste"; break;
			case KeyCode::Find: stream << "KeyCode::Find"; break;
			case KeyCode::Mute: stream << "KeyCode::Mute"; break;
			case KeyCode::VolumeUp: stream << "KeyCode::VolumeUp"; break;
			case KeyCode::VolumeDown: stream << "KeyCode::VolumeDown"; break;
			case KeyCode::Numlock: stream << "KeyCode::Numlock"; break;
			case KeyCode::KeypadComma: stream << "KeyCode::KeypadComma"; break;
			case KeyCode::KeypadEqualSign: stream << "KeyCode::KeypadEqualSign"; break;
			case KeyCode::LeftControl: stream << "KeyCode::LeftControl"; break;
			case KeyCode::LeftShift: stream << "KeyCode::LeftShift"; break;
			case KeyCode::LeftAlt: stream << "KeyCode::LeftAlt"; break;
			case KeyCode::LeftGui: stream << "KeyCode::LeftGui"; break;
			case KeyCode::RightControl: stream << "KeyCode::RightControl"; break;
			case KeyCode::RightShift: stream << "KeyCode::RightShift"; break;
			case KeyCode::RightAlt: stream << "KeyCode::RightAlt"; break;
			case KeyCode::RightGui: stream << "KeyCode::RightGui"; break;
			default:
				debug_log_error("KeyCode can't be formatted into a string");
		}
		return stream;
	}

	std::ostream& operator <<(std::ostream& stream, MouseButton v)
	{
		switch(v)
		{
			case MouseButton::Left: stream << "MouseButton::Left"; break;
			case MouseButton::Right: stream << "MouseButton::Right"; break;
			case MouseButton::Middle: stream << "MouseButton::Middle"; break;
			case MouseButton::Backward: stream << "MouseButton::Backward"; break;
			case MouseButton::Forward: stream << "MouseButton::Forward"; break;
		}
		return stream;
	}

	std::ostream&& operator <<(std::ostream&& stream, MouseButton v)
	{
		auto& _stream = operator <<(stream, v);
		return std::move(_stream);
	}
	
	std::ostream& operator <<(std::ostream& stream, KeyEvent v)
	{
		switch(v)
		{
			case KeyEvent::Press: stream << "KeyEvent::Press"; break;
			case KeyEvent::Release: stream << "KeyEvent::Release"; break;
		}
		return stream;
	}

	std::ostream&& operator <<(std::ostream&& stream, KeyEvent v)
	{
		auto& _stream = operator <<(stream, v);
		return std::move(_stream);
	}

}