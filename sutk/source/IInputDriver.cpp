#include <sutk/IInputDriver.hpp>

namespace SUTK
{
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