#include <sutk/IInputDriver.hpp>

// This typedef taken from glfw3.h
typedef struct GLFWwindow GLFWwindow;

namespace SUTK
{

	class SUTK_API GLFWInputDriver : public IInputDriver
	{
	private:
		GLFWwindow* m_windowPtr;
	public:

		GLFWInputDriver(GLFWwindow* windowPtr) noexcept;

		// Keyboard
		virtual bool getKey(KeyCode keycode) noexcept override;
		virtual bool getKeyDown(KeyCode keycode) noexcept override;
		virtual bool getKeyUp(KeyCode keycode) noexcept override;

		// Mouse
		virtual Vec2Df getMousePosition() noexcept override;
		virtual Vec2Df getMouseScrollDelta() noexcept override;
		virtual bool getMouseButton(MouseButton button) noexcept override;
		virtual bool getMouseButtonDown(MouseButton button) noexcept override;
		virtual bool getMouseButtonUp(MouseButton button) noexcept override;
	};
}