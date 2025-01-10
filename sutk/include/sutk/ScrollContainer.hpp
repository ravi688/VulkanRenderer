#pragma once

#include <sutk/defines.hpp>
#include <sutk/Container.hpp>
#include <sutk/InputEventHandlerObject.hpp>

namespace SUTK
{
	class Scrollable;
	class SUTK_API ScrollContainer : public SUTK::Container,
							public SUTK::MouseScrollHandlerObject,
							public SUTK::KeyboardHandlerObject
	{
		friend class Scrollable;
		private:
			SUTK::ModifierKeys m_modifiers;
			// In centimeters
			f32 m_scaleFactor;
			u32 m_numKeysPressed;
			Vec2Df m_scrollDelta;
			Scrollable* m_scrollable { };
			Compass4D<com::Bool> getScrollableCompass() const noexcept;
			void onScrollableAdd(Scrollable* scrollable) noexcept;
		protected:
			// Overrides of SUTK::MouseScrollHandlerObject
			virtual bool onMouseScroll(SUTK::Vec2Df scrollDelta) override;
			// Overrides of SUTK::KeyboardHandlerObject
			virtual void onKey(SUTK::KeyCode keycode, SUTK::KeyEvent event, SUTK::ModifierKeys modifiers) override;
		 public:
		 	ScrollContainer(UIDriver& driver, Container* parent = NULL) noexcept;

		 	// direction: +ve means towards right, -ve means towards left
		 	com::Bool isScrollableHorziontal(f32 direction) const noexcept;
		 	// direction: +ve means downwards, -ve means upwards
		 	com::Bool isScrollableVertical(f32 direction) const noexcept;
		 	com::Bool isScrollableLeft() const noexcept { return isScrollableHorziontal(-1); };
		 	com::Bool isScrollableRight() const noexcept { return isScrollableHorziontal(1); }
		 	com::Bool isScrollableUp() const noexcept { return isScrollableVertical(-1); }
		 	com::Bool isScrollableDown() const noexcept { return isScrollableVertical(1); }

		 	void setScaleFactor(f32 scaleFactor) noexcept { m_scaleFactor = scaleFactor; }
		 	f32 getScaleFactor() const noexcept { return m_scaleFactor; }
		 	// scrollDelta must be in centimeters
		 	void addScrollDelta(Vec2Df scrollDelta) noexcept;
		 	Vec2Df getScrollDelta() const noexcept { return m_scrollDelta; }
			// The internal accrued scroll delta becomes zero, so all the previous scroll will be reset to zero.
			void resetScrollDelta() noexcept;
	};
}
