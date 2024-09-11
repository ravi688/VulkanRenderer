#include <sutk/Button.hpp>

#include <sutk/ButtonGraphic.hpp>

namespace SUTK
{
	Button::Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic) noexcept : Container(driver, parent),
																									MouseMoveHandlerObject(driver, this),
																									MouseClickHandlerObject(driver, this),
																									m_graphic(NULL),
																									m_onEnterEvent(this),
																									m_onExitEvent(this),
																									m_onPressEvent(this),
																									m_onReleaseEvent(this)
	{ 
		if(isCreateDefaultGraphic)
			m_graphic = driver.createContainer<DefaultButtonGraphic>(this);
		enableMouseEnter(true);
		enableMouseExit(true);
	}

	void Button::onMouseEnter() noexcept
	{
		if(m_graphic != NULL)
		{
			m_onEnterEvent.publish();
			HoverInfo info { };
			info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
			info.isEnter = true;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseExit() noexcept
	{
		if(m_graphic != NULL)
		{
			m_onExitEvent.publish();
			HoverInfo info { };
			info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
			info.isExit = true;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseMove(Vec2Df position) noexcept
	{
		if(m_graphic != NULL)
		{
			HoverInfo info { };
			info.position = position;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseClick(MouseButton button, KeyEvent action) noexcept
	{
		if(button != MouseButton::Left)
			return;
		if(action == KeyEvent::Press)
		{
			m_onPressEvent.publish();
			if(m_graphic != NULL)
				m_graphic->onPress();
		}
		else if(action == KeyEvent::Release)
		{
			m_onReleaseEvent.publish();
			if(m_graphic != NULL)
				m_graphic->onRelease();
		}
	}
}