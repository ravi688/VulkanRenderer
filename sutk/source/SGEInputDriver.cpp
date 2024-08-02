#include <sutk/SGEInputDriver.hpp>
#include <sge-cpp/sge.hpp>
#include <sge-cpp/Display.hpp>
#include <GLFW/glfw3.h>

namespace SUTK
{
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