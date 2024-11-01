#include <sutk/Button.hpp>

#include <sutk/ButtonGraphic.hpp>

namespace SUTK
{
	Button::Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic, GfxDriverObjectHandleType textGroup) noexcept : Container(driver, parent),
																									MouseMoveHandlerObject(driver, this),
																									MouseClickHandlerObject(driver, this),
																									m_graphic(NULL),
																									m_onEnterEvent(NULL),
																									m_onExitEvent(NULL),
																									m_onPressEvent(NULL),
																									m_onReleaseEvent(NULL)
	{ 
		if(isCreateDefaultGraphic)
			setGraphic(driver.createContainer<DefaultButtonGraphic>(this, textGroup));
	}

	Button::Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic) noexcept : Container(driver, parent),
																									MouseMoveHandlerObject(driver, this),
																									MouseClickHandlerObject(driver, this),
																									m_graphic(NULL),
																									m_onEnterEvent(NULL),
																									m_onExitEvent(NULL),
																									m_onPressEvent(NULL),
																									m_onReleaseEvent(NULL)
	{ 
		if(isCreateDefaultGraphic)
			setGraphic(driver.createContainer<DefaultButtonGraphic>(this));
	}

	Button::~Button() noexcept
	{
		if(!m_onEnterEvent)
			delete m_onEnterEvent;
		if(!m_onExitEvent)
			delete m_onExitEvent;
		if(!m_onPressEvent)
			delete m_onPressEvent;
		if(!m_onReleaseEvent)
			delete m_onReleaseEvent;
	}

	void Button::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		Container::onResize(newRect, isPositionChanged, isSizeChanged);
		MouseMoveHandlerObject::update();
	}

	bool Button::onMouseMove(MouseMoveEvent event, Vec2Df position) noexcept
	{
		if(event == MouseMoveEvent::Move)
		{
			if(m_graphic != NULL)
			{
				HoverInfo info { };
				info.position = position;
				m_graphic->onHover(info);
			}
		}
		else if(event == MouseMoveEvent::Exit)
		{
			if(m_onExitEvent != NULL)
				m_onExitEvent->publish();
			if(m_graphic != NULL)
			{
				HoverInfo info { };
				info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
				info.isExit = true;
				m_graphic->onHover(info);
			}
		}
		else if(event == MouseMoveEvent::Enter)
		{			
			if(m_onEnterEvent != NULL)
				m_onEnterEvent->publish();
			if(m_graphic != NULL)
			{
				HoverInfo info { };
				info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
				info.isEnter = true;
				m_graphic->onHover(info);
			}
		}
		return true;
	}

	bool Button::onMouseClick(MouseButton button, KeyEvent action) noexcept
	{
		if(button != MouseButton::Left)
			return true;
		if(action == KeyEvent::Press)
		{
			if(m_onPressEvent != NULL)
				m_onPressEvent->publish();
			if(m_graphic != NULL)
				m_graphic->onPress();
		}
		else if(action == KeyEvent::Release)
		{
			if(m_onReleaseEvent != NULL)
				m_onReleaseEvent->publish();
			if(m_graphic != NULL)
				m_graphic->onRelease();
		}
		return true;
	}

	void Button::checkAndOptimizeEvents() noexcept
	{
		bool isEnterEnabled = isMouseEnterEnabled();
		bool isExitEnabled = isMouseExitEnabled();
		if(m_graphic)
		{
			if(!isEnterEnabled)
				enableMouseEnter(true);
			if(!isExitEnabled)
				enableMouseExit(true);
		}
		else
		{
			if(!isEnterEnabled && m_onEnterEvent)
				enableMouseEnter(true);
			else if(isEnterEnabled && !m_onEnterEvent)
				enableMouseEnter(false);

			if(!isExitEnabled && m_onExitEvent)
				enableMouseExit(true);
			else if(isExitEnabled && !m_onExitEvent)
				enableMouseExit(false);
		}
	}

	void Button::clearAllEvents() noexcept
	{
		if(hasOnPressEvent())
			getOnPressEvent().clear();
		if(hasOnReleaseEvent())
			getOnReleaseEvent().clear();
		if(hasOnEnterEvent())
			getOnEnterEvent().clear();
		if(hasOnExitEvent())
			getOnExitEvent().clear();
	}

	void Button::setGraphic(IButtonGraphic* graphic) noexcept
	{
		m_graphic = graphic;
		checkAndOptimizeEvents();
	}

	Button::OnEnterEvent& Button::getOnEnterEvent() noexcept
	{ 
		if(!m_onEnterEvent)
		{
			m_onEnterEvent = new OnEnterEvent(this);
			checkAndOptimizeEvents();
		}
		return *m_onEnterEvent;
	}
	Button::OnExitEvent& Button::getOnExitEvent() noexcept
	{ 
		if(!m_onExitEvent)
		{
			m_onExitEvent = new OnExitEvent(this);
			checkAndOptimizeEvents();
		}
		return *m_onExitEvent;
	}
	Button::OnPressEvent& Button::getOnPressEvent() noexcept
	{ 
		if(!m_onPressEvent)
		{
			m_onPressEvent = new OnPressEvent(this);
			checkAndOptimizeEvents();
		}
		return *m_onPressEvent;
	}
	Button::OnReleaseEvent& Button::getOnReleaseEvent() noexcept
	{ 
		if(!m_onReleaseEvent)
		{
			m_onReleaseEvent = new OnReleaseEvent(this);
			checkAndOptimizeEvents();
		}
		return *m_onReleaseEvent;
	}
}