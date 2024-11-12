#include <sutk/InputEventHandlerObject.hpp>
#include <sutk/UIDriver.hpp>

namespace SUTK
{
	MouseEventsBlockerObject::MouseEventsBlockerObject(UIDriver& driver, Container* container, Layer layer) noexcept : InputEventContainerAux(container), m_uiDriver(driver)
	{
		_com_assert(container != NULL);

		OrderedInputEventsDispatcher& eventsDispatcher = driver.getOrderedInputEventsDispatcher();

		m_mouseMoveID = eventsDispatcher.getOnCursorMoveEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df pos)
		{
			if(isInside(pos))
				// Block containers from receiving this event being behind this container (parents/anscestors)
				return true;
			return false;
		}, (layer == InvalidLayer) ? container->getDepth() : layer);
		m_mouseButtonID = eventsDispatcher.getOnMouseButtonEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			Vec2Df pos = this->m_uiDriver.getInputDriver().getMousePosition();
			if(isInside(pos))
				// Block containers from receiving this event being behind this container (parents/anscestors)
				return true;
			return false;
		}, (layer == InvalidLayer) ? container->getDepth() : layer);
		m_mouseScrollID = eventsDispatcher.getOnScrollEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df delta)
		{
			Vec2Df pos = this->m_uiDriver.getInputDriver().getMousePosition();
			if(isInside(pos))
				// Block containers from receiving this event being behind this container (parents/anscestors)
				return true;
			return false;
		}, (layer == InvalidLayer) ? container->getDepth() : layer);

		// If the container has been deactivated then sleep the subscription,
		// Or if the container has been activated then awake the subscription
		// This saves CPU cycles, as well as avoids receiving input events in disabled state.
		container->getOnActiveEvent().subscribe([this](com::Bool isActive)
		{
			OrderedInputEventsDispatcher& eventsDispatcher = this->m_uiDriver.getOrderedInputEventsDispatcher();
			if(isActive)
			{
				eventsDispatcher.getOnCursorMoveEvent().activate(this->m_mouseMoveID);
				eventsDispatcher.getOnMouseButtonEvent().activate(this->m_mouseButtonID);
				eventsDispatcher.getOnScrollEvent().activate(this->m_mouseScrollID);
			}
			else
			{
				eventsDispatcher.getOnCursorMoveEvent().deactivate(this->m_mouseMoveID);
				eventsDispatcher.getOnMouseButtonEvent().deactivate(this->m_mouseButtonID);
				eventsDispatcher.getOnScrollEvent().deactivate(this->m_mouseScrollID);
			}
		});
	}

	MouseEventsBlockerObject::~MouseEventsBlockerObject() noexcept
	{
		OrderedInputEventsDispatcher& eventsDispatcher = m_uiDriver.getOrderedInputEventsDispatcher();
		eventsDispatcher.getOnCursorMoveEvent().unsubscribe(m_mouseMoveID);
		eventsDispatcher.getOnMouseButtonEvent().unsubscribe(m_mouseButtonID);
		eventsDispatcher.getOnScrollEvent().unsubscribe(m_mouseScrollID);
	}

	GlobalMouseMoveHandlerObject::GlobalMouseMoveHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnMouseMoveEvent>(driver.getInputDriver().getOnCursorMoveEvent())
	{
		IInputDriver::OnMouseMoveEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df pos) noexcept
		{
			onMouseMove(pos);
		});
		setSubscriptionID(id);
	}

	MouseMoveHandlerObject::MouseMoveHandlerObject(UIDriver& driver, Container* container) noexcept : TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseMoveEvent>(driver.getInputDriver(), driver.getOrderedInputEventsDispatcher().getOnCursorMoveEvent(), container),
																									  m_isMouseEnterEnabled(false),
																									  m_isMouseExitEnabled(false),
																									  m_isMouseMoveOutsideEnabled(false),
																									  m_isInside(false)
	{
		OrderedInputEventsDispatcher::OnMouseMoveEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, SUTK::Vec2Df position)
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
		else if(m_isInside || m_isMouseMoveOutsideEnabled)
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
		OrderedInputEventsDispatcher::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
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
		OrderedInputEventsDispatcher::OnMouseScrollEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df scrollDelta)
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