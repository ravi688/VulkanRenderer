#include <sutk/ScrollContainer.hpp>
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

	com::Bool ScrollContainer::isLeftScrollable() noexcept
	{
		return com::True;
	}

	com::Bool ScrollContainer::isRightScrollable() noexcept
	{
		return com::Bool { m_scrollDelta.x < Vec2Df::zero().x };
	}

	void ScrollContainer::addScrollDelta(Vec2Df scrollDelta) noexcept
	{
		std::vector<Container*> childs = getChilds();
		for(Container* child : childs)
		{
			Vec2Df position = child->getPosition();
			Vec2Df br = child->getRect().getBottomRight();
			Vec2Df tl = child->getRect().getTopLeft();
			if((scrollDelta.y + tl.y) > 0.0f)
				scrollDelta.y = -tl.y;
			else if((scrollDelta.y + br.y) < 0.0f)
				scrollDelta.y = 0.0f;
			if((scrollDelta.x + tl.x) > 0.0f)
				scrollDelta.x = -tl.x;
			else if((scrollDelta.x + br.x) < 0.0f)
				scrollDelta.x = 0.0f;
			m_scrollDelta += scrollDelta;
			child->setPosition(scrollDelta + position);
		}
	}
}