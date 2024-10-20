#include <sutk/OrderedInputEventsDispatcher.hpp>

#include <sutk/IInputDriver.hpp> // for IInputDriver

namespace SUTK
{
	OrderedInputEventsDispatcher::OrderedInputEventsDispatcher(IInputDriver& inputDriver) noexcept : m_inputDriver(inputDriver),
																									m_onCursorMoveEvent(&inputDriver),
																									m_onMouseButtonEvent(&inputDriver),
																									m_onScrollEvent(&inputDriver),
																									m_onKeyEvent(&inputDriver),
																									m_onMouseEnterExitEvent(&inputDriver)
	{
		m_OnCursorMoveEventID = inputDriver.getOnCursorMoveEvent().subscribe([this](IInputDriver* driver, Vec2Df position)
		{
			this->m_onCursorMoveEvent.publish(position);
		});
		m_OnMouseButtonEventID = inputDriver.getOnMouseButtonEvent().subscribe([this](IInputDriver* driver, MouseButton button, KeyEvent keyEvent)
		{
			this->m_onMouseButtonEvent.publish(button, keyEvent);
		});
		m_OnScrollEventID = inputDriver.getOnScrollEvent().subscribe([this](IInputDriver* driver, Vec2Df delta)
		{
			this->m_onScrollEvent.publish(delta);
		});
		m_OnKeyEventID = inputDriver.getOnKeyEvent().subscribe([this](IInputDriver* driver, KeyCode keyCode, KeyEvent keyEvent, ModifierKeys modifierKeys)
		{
			this->m_onKeyEvent.publish(keyCode, keyEvent, modifierKeys);
		});
		m_OnMouseEnterExitEventID = inputDriver.getOnMouseEnterExitEvent().subscribe([this](IInputDriver* driver, bool isEnter)
		{
			this->m_onMouseEnterExitEvent.publish(isEnter);
		});
	}

	OrderedInputEventsDispatcher::~OrderedInputEventsDispatcher() noexcept
	{
		m_inputDriver.getOnCursorMoveEvent().unsubscribe(m_OnCursorMoveEventID);
		m_inputDriver.getOnMouseButtonEvent().unsubscribe(m_OnMouseButtonEventID);
		m_inputDriver.getOnScrollEvent().unsubscribe(m_OnScrollEventID);
		m_inputDriver.getOnKeyEvent().unsubscribe(m_OnKeyEventID);
		m_inputDriver.getOnMouseEnterExitEvent().unsubscribe(m_OnMouseEnterExitEventID);
	}
}