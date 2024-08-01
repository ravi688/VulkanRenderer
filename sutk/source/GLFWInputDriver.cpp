#include <sutk/GLFWInputDriver.hpp>

#include <GLFW/glfw3.h>

namespace SUTK
{
	GLFWInputDriver::GLFWInputDriver(GLFWwindow* windowPtr) noexcept : m_windowPtr(windowPtr)
	{
		
	}

	// Keyboard
	bool GLFWInputDriver::getKey(KeyCode keycode) noexcept
	{
		return false;
	}

	bool GLFWInputDriver::getKeyDown(KeyCode keycode) noexcept
	{
		return false;
	}

	bool GLFWInputDriver::getKeyUp(KeyCode keycode) noexcept
	{
		return false;
	}


	// Mouse
	Vec2Df GLFWInputDriver::getMousePosition() noexcept
	{
		return { };
	}

	Vec2Df GLFWInputDriver::getMouseScrollDelta() noexcept
	{
		return { };
	}

	bool GLFWInputDriver::getMouseButton(MouseButton button) noexcept
	{
		return false;
	}

	bool GLFWInputDriver::getMouseButtonDown(MouseButton button) noexcept
	{
		return false;
	}

	bool GLFWInputDriver::getMouseButtonUp(MouseButton button) noexcept
	{
		return false;
	}

}