#include <sutk/IInputDriver.hpp> 

namespace SUTK
{
	class SUTK_API DummyInputDriver : public IInputDriver
	{
	public:
		// Keyboard
		virtual bool getKey(KeyCode keycode) noexcept override { return false; }
		virtual bool getKeyDown(KeyCode keycode) noexcept override { return false; }
		virtual bool getKeyUp(KeyCode keycode) noexcept override { return false; }

		// Mouse
		virtual Vec2Df getMousePosition() const noexcept override { return { }; }
		virtual Vec2Df getMouseScrollDelta() noexcept override { return { }; }
		virtual bool getMouseButton(MouseButton button) noexcept override { return false; }
		virtual bool getMouseButtonDown(MouseButton button) noexcept override { return false; }
		virtual bool getMouseButtonUp(MouseButton button) noexcept override { return false; }
	};
}
