#pragma once

#include <sutk/defines.hpp>
#include <sutk/Container.hpp>
#include <sutk/InputEventHandlerObject.hpp>

namespace SUTK
{
	class ScrollContainer : public SUTK::Container,
							public SUTK::MouseScrollHandlerObject,
							public SUTK::MouseMoveHandlerObject,
							public SUTK::KeyboardHandlerObject
	{
		private:
			bool m_isInside;
			SUTK::ModifierKeys m_modifiers;
			// In centimeters
			f32 m_scaleFactor;
			u32 m_numKeysPressed;
		protected:
			// Overrides of SUTK::MouseScrollHandlerObject
			virtual void onMouseScroll(SUTK::Vec2Df scrollDelta) override;
			// Overrides of SUTK::MouseMoveHandlerObject
			virtual void onMouseMove(SUTK::Vec2Df position) override { }
			virtual void onMouseEnter() noexcept override;
			virtual void onMouseExit() noexcept override;
			// Overrides of SUTK::KeyboardHandlerObject
			virtual void onKey(SUTK::KeyCode keycode, SUTK::KeyEvent event, SUTK::ModifierKeys modifiers) override;
		 public:
		 	ScrollContainer(UIDriver& driver, Container* parent = NULL) noexcept;

		 	void setScaleFactor(f32 scaleFactor) noexcept { m_scaleFactor = scaleFactor; }
		 	f32 getScaleFactor() const noexcept { return m_scaleFactor; }
		 	// scrollDelta must be in centimeters
		 	void addScrollDelta(Vec2Df scrollDelta) noexcept;
	};
}