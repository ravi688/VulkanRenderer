#include <sutk/InputEventHandlerObject.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK
{
	MouseEnterExitHandlerObject::MouseEnterExitHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnMouseEnterExitEvent>(driver.getInputDriver().getOnMouseEnterExitEvent())
	{
		IInputDriver::OnMouseEnterExitEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, bool isEnter)
		{
			if(isEnter)
				onMouseEnter();
			else
				onMouseExit();
		});
		setSubscriptionID(id);
	}

	MouseMoveHandlerObject::MouseMoveHandlerObject(UIDriver& driver, Container* container) noexcept : TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseMoveEvent>(driver.getInputDriver(), driver.getOrderedInputEventsDispatcher().getOnCursorMoveEvent(), container),
																									  m_isMouseEnterEnabled(false),
																									  m_isMouseExitEnabled(false),
																									  m_isInside(false)
	{
		IInputDriver::OnMouseMoveEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, SUTK::Vec2Df position)
		{
			return this->update(position);
		}, container ? container->getDepth() : std::numeric_limits<u32>::max());
		setSubscriptionID(id);
	}

	bool MouseMoveHandlerObject::update(Vec2Df position) noexcept
	{
		if((!m_isInside) && isInside(position))
		{
			m_isInside = true;
			if(m_isMouseEnterEnabled)
				return onMouseMove(MouseMoveEvent::Enter, globalToLocalCoords(position));
		}
		else if(m_isInside && (!isInside(position)))
		{
			m_isInside = false;
			if(m_isMouseExitEnabled)
				return onMouseMove(MouseMoveEvent::Exit, globalToLocalCoords(position));
		}
		else if(m_isInside)
			return onMouseMove(MouseMoveEvent::Move, globalToLocalCoords(position));
		return false;
	}

	void MouseMoveHandlerObject::update() noexcept
	{
		 Vec2Df position = m_inputDriver.getMousePosition();
		 update(position);
	}

	void MouseMoveHandlerObject::sleep() noexcept
	{
		TInputEventHandlerContainerObject::sleep();
		if(m_isMouseExitEnabled && m_isInside)
		{
			onMouseMove(MouseMoveEvent::Exit, globalToLocalCoords(m_inputDriver.getMousePosition()));
			m_isInside = false;
		}
	}

	void MouseMoveHandlerObject::awake() noexcept
	{
		TInputEventHandlerContainerObject::awake();
		if(m_isMouseEnterEnabled)
		{
			Vec2Df position = m_inputDriver.getMousePosition();
			if(isInside(position))
			{
				m_isInside = true;
				onMouseMove(MouseMoveEvent::Enter, globalToLocalCoords(position));
			}
		}
	}

	MouseClickHandlerObject::MouseClickHandlerObject(UIDriver& driver, Container* container) noexcept : TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseButtonEvent>(driver.getInputDriver(), driver.getOrderedInputEventsDispatcher().getOnMouseButtonEvent(), container)
	{
		IInputDriver::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			if(isInside(inputDriver->getMousePosition()))
				return onMouseClick(button, event);
			return false;
		}, container ? container->getDepth() : std::numeric_limits<u32>::max());
		setSubscriptionID(id);
	}

	MouseAnyClickHandlerObject::MouseAnyClickHandlerObject(UIDriver& driver, Container* container) noexcept : TMouseEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>(driver.getInputDriver(), driver.getInputDriver().getOnMouseButtonEvent(), container)
	{
		IInputDriver::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			this->onMouseClick(button, event, isInside(inputDriver->getMousePosition()));
		});
		setSubscriptionID(id);
	}

	MouseScrollHandlerObject::MouseScrollHandlerObject(UIDriver& driver, Container* container) noexcept : TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseScrollEvent>(driver.getInputDriver(), driver.getOrderedInputEventsDispatcher().getOnScrollEvent(), container)
	{
		IInputDriver::OnMouseScrollEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df scrollDelta)
		{
			if(isInside(inputDriver->getMousePosition()))
				return onMouseScroll(scrollDelta);
			return false;
		}, container ? container->getDepth() : std::numeric_limits<u32>::max());
		setSubscriptionID(id);
	}

	KeyboardHandlerObject::KeyboardHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnKeyboardEvent>(driver.getInputDriver().getOnKeyEvent())
	{
		IInputDriver::OnKeyboardEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, KeyCode keycode, KeyEvent action, ModifierKeys modifiers)
		{
			onKey(keycode, action, modifiers);
		});
		setSubscriptionID(id);
	}	
}