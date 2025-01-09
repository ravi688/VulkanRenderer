#pragma once

#include <sutk/defines.hpp>
#include <sutk/Container.hpp>
#include <sutk/InputEventHandlerObject.hpp>

namespace SUTK
{
	class ScrollContainer : public SUTK::Container,
							public SUTK::MouseScrollHandlerObject,
							public SUTK::KeyboardHandlerObject
	{
		private:
			SUTK::ModifierKeys m_modifiers;
			// In centimeters
			f32 m_scaleFactor;
			u32 m_numKeysPressed;
			Vec2Df m_scrollDelta;
		protected:
			// Overrides of SUTK::MouseScrollHandlerObject
			virtual bool onMouseScroll(SUTK::Vec2Df scrollDelta) override;
			// Overrides of SUTK::KeyboardHandlerObject
			virtual void onKey(SUTK::KeyCode keycode, SUTK::KeyEvent event, SUTK::ModifierKeys modifiers) override;
		 public:
		 	ScrollContainer(UIDriver& driver, Container* parent = NULL) noexcept;

		 	// direction: 1 means towards right, -1 means towards left
		 	com::Bool isScrollableTowards(f32 direction) noexcept { return (direction > 0) ? isRightScrollable() : isLeftScrollable(); }
		 	com::Bool isLeftScrollable() noexcept;
		 	com::Bool isRightScrollable() noexcept;

		 	void setScaleFactor(f32 scaleFactor) noexcept { m_scaleFactor = scaleFactor; }
		 	f32 getScaleFactor() const noexcept { return m_scaleFactor; }
		 	// scrollDelta must be in centimeters
		 	void addScrollDelta(Vec2Df scrollDelta) noexcept;
		 	Vec2Df getScrollDelta() const noexcept { return m_scrollDelta; }
	};
}
