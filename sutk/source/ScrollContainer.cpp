#include <sutk/ScrollContainer.hpp>
#include <sutk/Scrollable.hpp>
#include <common/assert.h> // for _com_assert
#include <common/debug.hpp> // for com::source_loc()

namespace SUTK
{
	ScrollContainer::ScrollContainer(UIDriver& driver, Container* parent) noexcept : Container(driver, parent),
																					MouseScrollHandlerObject(driver, this),
																					KeyboardHandlerObject(driver),
																					m_scaleFactor(0.5f),
																					m_numKeysPressed(0),
																					m_scrollDelta({ 0.0f, 0.0f })
	{
	}

	bool ScrollContainer::onMouseScroll(SUTK::Vec2Df scrollDelta)
	{
		if(!isMousePosInside())
		{
			std::cout << "Not Inside" << std::endl;
			return true;
		}
		
		// If more than 1 key is pressed then we definitely do nothing
		// If only one key is pressed then it must be only shift, otherwise do nothing
		if((m_numKeysPressed > 1) || ((m_numKeysPressed == 1) && !m_modifiers.shift()))
			return true;

		// Calculate the scroll delta in centimeters and the direction of the scroll
		auto deltaInCentimeters = scrollDelta * m_scaleFactor;
		if(((scrollDelta.x * scrollDelta.y) == 0.0f) && m_modifiers.shift())
			deltaInCentimeters = SUTK::Vec2Df(deltaInCentimeters.y, deltaInCentimeters.x);
		
		// Apply the scroll delta values
		addScrollDelta(deltaInCentimeters);
		return true;
	}

	void ScrollContainer::onKey(SUTK::KeyCode keycode, SUTK::KeyEvent event, SUTK::ModifierKeys modifiers)
	{
		m_modifiers = modifiers;
		if(event == SUTK::KeyEvent::Press)
			++m_numKeysPressed;
		else if(event == SUTK::KeyEvent::Release)
		{
			_com_assert(m_numKeysPressed > 0);
			--m_numKeysPressed;
		}
	}

	void ScrollContainer::onScrollableAdd(Scrollable* scrollable) noexcept
	{
		com_assert(COM_DESCRIPTION(!m_scrollable), "Only one child (Scrollable) can be added into a Scrollable Container, you need to first remove the older child");
		m_scrollable = scrollable;
	}

	Compass4D<com::Bool> ScrollContainer::getScrollableCompass() const noexcept
	{
		// If there is nothing to scroll then all directions are non-scrollable
		if(!m_scrollable)
			return { };

		auto size = getSize();
		auto margins = m_scrollable->getScrollMargins();
		auto rect = m_scrollable->getContainer()->getRect();

		Compass4D<com::Bool> cp;

		// Can only be scrolled downwards if the distance of the top edge of the Scrollable's Rect from the top edge of the Scroll Container's Rect is less than the top margin 
		auto topDistance = rect.getTop();
		cp.down = com::Bool { topDistance < margins.top };

		// Can only be scrolled upwards if the distance of the bottom edge of the Scrollable's Rect from the bottom edge of the Scroll Container's Rect is less than the bottom margin
		auto bottomDistance = size.height - rect.getBottom();
		cp.up = com::Bool { bottomDistance < margins.bottom };

		// Can only be scrolled towards right if the distance of the left edge of the Scrollable's Rect from the left edge of the Scroll Container's Rect is less than the left margin 
		auto leftDistance = rect.getLeft();;
		cp.right = com::Bool { leftDistance < margins.left };

		// Can only be scrolled towards left if the distance of the right edge of the Scrollable's Rect from the right edge of the Scroll Container's Rect is less than the right margin
		auto rightDistance = size.width - rect.getRight();
		cp.left = com::Bool { rightDistance < margins.right };

		return cp;
	}

	com::Bool ScrollContainer::isScrollableHorziontal(f32 direction) const noexcept
	{
		auto cp = getScrollableCompass();
		if(direction > 0.0f) return cp.right;
		return cp.left;
	}

	com::Bool ScrollContainer::isScrollableVertical(f32 direction) const noexcept
	{
		auto cp = getScrollableCompass();
		if(direction > 0.0f) return cp.bottom;
		return cp.top;
	}

	void ScrollContainer::addScrollDelta(Vec2Df scrollDelta) noexcept
	{
		// If there is nothing to scroll (no childs) then do nothing
		if(!m_scrollable)
			return;

		auto size = getSize();
		
		auto margins = m_scrollable->getScrollMargins();
		auto rect = m_scrollable->getContainer()->getRect();
		Vec2Df position = rect.getPosition();

		// Local coordinates of top, bottom, left, and right sides in their respective ordinate or abscissa
		auto top = rect.getTop();
		auto bottom = rect.getBottom();
		auto left = rect.getLeft();
		auto right = rect.getRight();

		// Constrain Vertical Motion
		// If scrolling downwards and the top distance is greater than the top scroll margin, then constrain the scroll
		if(auto topDistance = top + scrollDelta.y; topDistance > margins.top && scrollDelta.y > 0)
			scrollDelta.y = margins.top - top;
		// If scrolling upwards and the bottom distance is greater than the bottom scroll margin, then constrain the scroll
		else if(auto bottomDistance = size.height - (bottom + scrollDelta.y); bottomDistance > margins.bottom && scrollDelta.y < 0)
			scrollDelta.y = -(margins.bottom - (size.height - bottom));
			
		// Constrain Horizontal Motion
		// If scrolling towards right and the left distance is greater than the left scroll margin, then constrain the scroll
		if(auto leftDistance = left + scrollDelta.x; leftDistance > margins.left && scrollDelta.x > 0)
			scrollDelta.x = margins.left - left;
		// If scrolling towards left and the right distance is greater than the right scroll margin, then constrain the scroll
		else if(auto rightDistance = size.width - (right + scrollDelta.x); rightDistance > margins.right && scrollDelta.x < 0)
			scrollDelta.x = -(margins.right - (size.width - right));

		// Save the accrued scroll delta
		m_scrollDelta += scrollDelta;
		// Apply the scroll delta (new position)
		m_scrollable->getContainer()->setPosition(scrollDelta + position);
	}

	void ScrollContainer::resetScrollDelta() noexcept
	{
		addScrollDelta(-getScrollDelta());
	}
}