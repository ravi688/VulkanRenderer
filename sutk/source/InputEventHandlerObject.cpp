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

	MouseMoveHandlerObject::MouseMoveHandlerObject(UIDriver& driver, Container* container) noexcept : TInputEventHandlerContainerObject<IInputDriver::OnMouseMoveEvent>(driver.getInputDriver().getOnCursorMoveEvent(), container),
																									  m_isMouseEnterEnabled(false),
																									  m_isMouseExitEnabled(false),
																									  m_isInside(false),
																									  m_inputDriver(driver.getInputDriver())
	{
		IInputDriver::OnMouseMoveEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, SUTK::Vec2Df position)
		{
			if((!m_isInside) && isInside(position))
			{
				m_isInside = true;
				if(m_isMouseEnterEnabled)
					onMouseEnter();
			}
			else if(m_isInside && (!isInside(position)))
			{
				m_isInside = false;
				if(m_isMouseExitEnabled)
					onMouseExit();
			}
			
			if(m_isInside)
			{
				// convert global coordinates to the local coordinates of the container 'container'
				Container* container = getContainer();
				if(container != NULL)
					position = container->getScreenCoordsToLocalCoords(position);
				onMouseMove(position);
			}
		});
		setSubscriptionID(id);
	}

	void MouseMoveHandlerObject::sleep() noexcept
	{
		TInputEventHandlerContainerObject::sleep();
		if(m_isMouseExitEnabled && m_isInside)
		{
			onMouseExit();
			m_isInside = false;
		}
	}

	void MouseMoveHandlerObject::awake() noexcept
	{
		TInputEventHandlerContainerObject::awake();
		if(m_isMouseEnterEnabled && isInside(m_inputDriver.getMousePosition()))
		{
			m_isInside = true;
			onMouseEnter();
		}
	}

	MouseClickHandlerObject::MouseClickHandlerObject(UIDriver& driver, Container* container) noexcept : TInputEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>(driver.getInputDriver().getOnMouseButtonEvent(), container)
	{
		IInputDriver::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			if(isInside(inputDriver->getMousePosition()))
				onMouseClick(button, event);
		});
		setSubscriptionID(id);
	}

	MouseAnyClickHandlerObject::MouseAnyClickHandlerObject(UIDriver& driver, Container* container) noexcept : TInputEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>(driver.getInputDriver().getOnMouseButtonEvent(), container)
	{
		IInputDriver::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			this->onMouseClick(button, event, isInside(inputDriver->getMousePosition()));
		});
		setSubscriptionID(id);
	}

	MouseScrollHandlerObject::MouseScrollHandlerObject(UIDriver& driver, Container* container) noexcept : TInputEventHandlerContainerObject<IInputDriver::OnMouseScrollEvent>(driver.getInputDriver().getOnScrollEvent(), container)
	{
		IInputDriver::OnMouseScrollEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df scrollDelta)
		{
			if(isInside(inputDriver->getMousePosition()))
				onMouseScroll(scrollDelta);
		});
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