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