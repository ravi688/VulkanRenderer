#include <sutk/IInputDriver.hpp>
#include <sge-cpp/RenderWindow.hpp>

namespace SGE
{
	class Driver;
}

namespace SUTK
{
	class SUTK_API SGEInputDriver : public IInputDriver
	{
	private:
		SGE::RenderWindow m_window;
	public:

		SGEInputDriver(SGE::Driver& driver) noexcept;

		// Keyboard
		virtual bool getKey(KeyCode keycode) noexcept override;
		virtual bool getKeyDown(KeyCode keycode) noexcept override;
		virtual bool getKeyUp(KeyCode keycode) noexcept override;

		// Mouse

		// returns mouse position relative to top-left corner of the window (in centi-meters)
		virtual Vec2Df getMousePosition() const noexcept override;
		virtual Vec2Df getMouseScrollDelta() noexcept override;
		virtual bool getMouseButton(MouseButton button) noexcept override;
		virtual bool getMouseButtonDown(MouseButton button) noexcept override;
		virtual bool getMouseButtonUp(MouseButton button) noexcept override;
	};
}