#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/IInputDriver.hpp> // for SUTK::MouseButton and SUTK::KeyEvent
#include <sutk/OrderedInputEventsDispatcher.hpp>
#include <sutk/Container.hpp> // for SUTK::Container

namespace SUTK
{
	class UIDriver;
	
	class IInputEventHandlerObject
	{
	public:

		// unsubscribes this handler from the corresponding input event, thus reduces overhead
		virtual void sleep() = 0;
		// subscribes this handler to corresponding input event, increases the overhead
		virtual void awake() = 0;
	};

	template<typename InputEventType>
	class TInputEventHandlerObject : public IInputEventHandlerObject
	{
	protected:
		InputEventType& m_event;
		typename InputEventType::SubscriptionID m_id;

		TInputEventHandlerObject(InputEventType& event) noexcept : m_event(event), m_id(InputEventType::InvalidSubscriptionID) { }

		void setSubscriptionID(typename InputEventType::SubscriptionID id) noexcept { m_id = id; }
		typename InputEventType::SubscriptionID getSubscriptionID() const noexcept { return m_id; }
		InputEventType& getEvent() { return m_event; }
	public:

		virtual ~TInputEventHandlerObject() noexcept
		{
			getEvent().unsubscribe(getSubscriptionID());
		}

		// Implementation of IInputEventHandlerObject
		// Mandatory to be called by the overriding method
		virtual void sleep() noexcept override
		{
			_com_assert(m_id != InputEventType::InvalidSubscriptionID);
			m_event.deactivate(m_id);
		}

		// Mandatory to be called by the overriding method
		virtual void awake() noexcept override
		{
			_com_assert(m_id != InputEventType::InvalidSubscriptionID);
			m_event.activate(m_id);
		}

		void grabExclusiveAccess() noexcept
		{
			_com_assert(m_id != InputEventType::InvalidSubscriptionID);
			m_event.grabExclusiveAccess(m_id);
		}

		void releaseExclusiveAccess() noexcept
		{
			_com_assert(m_event.getExclusiveAccessID() == m_id);
			m_event.releaseExclusiveAccess(m_id);
		}
	};

	class InputEventContainerAux
	{
	private:
		Container* m_container;
	protected:
		InputEventContainerAux(Container* container = NULL) noexcept : m_container(container) { }
		// point: is in global coordinates
		bool isInside(Vec2Df point) const noexcept
		{
			return (m_container != NULL) ? m_container->containsGlobalCoords(point) : true;
		}
		Container* getContainer() noexcept { return m_container; }
		Container* getContainer() const noexcept { return m_container; }
		Vec2Df globalToLocalCoords(Vec2Df localCoords) const noexcept
		{
			// convert global coordinates to the local coordinates of the container 'container'
			Container* container = getContainer();
			if(container != NULL)
				localCoords = container->getScreenCoordsToLocalCoords(localCoords);
			return localCoords;
		}
	};

	template<typename InputEventType>
	class TInputEventHandlerContainerObject : public InputEventContainerAux, public TInputEventHandlerObject<InputEventType>
	{
	protected:
		TInputEventHandlerContainerObject(InputEventType& event, Container* container = NULL) noexcept : InputEventContainerAux(container), TInputEventHandlerObject<InputEventType>(event)
		{
			if(container != NULL)
			{
				// If the container has been deactivated then sleep the subscription,
				// Or if the container has been activated then awake the subscription
				// This saves CPU cycles, as well as avoids receiving input events in disabled state.
				container->getOnActiveEvent().subscribe([this](com::Bool isActive)
				{
					if(isActive)
						this->awake();
					else
						this->sleep();
				});
			}
		}
	};

	template<typename InputEventType>
	class TMouseEventHandlerContainerObject : public TInputEventHandlerContainerObject<InputEventType>
	{
	protected:
		// There is a problem if we keep this into private scope and expose a protected getter 'getInputDriver()' function for it.
		// If a class derives from UIDriverObject and from this also, then the call to 'getInputDriver()' would be ambiguous.
		// Right now I can't figure out a solution for it.
		IInputDriver& m_inputDriver;
		TMouseEventHandlerContainerObject(IInputDriver& inputDriver, InputEventType& event) noexcept : TInputEventHandlerContainerObject<InputEventType>(event), m_inputDriver(inputDriver) { }
		TMouseEventHandlerContainerObject(IInputDriver& inputDriver, InputEventType& event, Container* container) noexcept : TInputEventHandlerContainerObject<InputEventType>(event, container), m_inputDriver(inputDriver) { }

		bool isMousePosInside() noexcept
		{
			Vec2Df position = m_inputDriver.getMousePosition();
			return TInputEventHandlerContainerObject<InputEventType>::isInside(position);
		}
	};

	class SUTK_API MouseEventsBlockerObject : public InputEventContainerAux
	{
	private:
		UIDriver& m_uiDriver;
		OrderedInputEventsDispatcher::OnMouseMoveEvent::SubscriptionID m_mouseMoveID;
		OrderedInputEventsDispatcher::OnMouseButtonEvent::SubscriptionID m_mouseButtonID;
		OrderedInputEventsDispatcher::OnMouseScrollEvent::SubscriptionID m_mouseScrollID;
	public:
		MouseEventsBlockerObject(UIDriver& driver, Container* container) noexcept;
		virtual ~MouseEventsBlockerObject() noexcept;
	};

	// The onMouseEnter and onMouseExit are called only for the render window's entire rect
	// Therefore, this class is not derived from TInputEventHandlerContainerObject
	// If you want to get callbacks on mouse enter and exit across the boundaries of a specific Container
	// then derive your class from MouseMoveHandlerObject and call at least once the enableMouseEnter and enableMouseExit with 'true' values
	// to enable the callbacks.
	class MouseEnterExitHandlerObject : public TInputEventHandlerObject<IInputDriver::OnMouseEnterExitEvent>
	{
	protected:
		virtual void onMouseEnter() = 0;
		virtual void onMouseExit() = 0;

		MouseEnterExitHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~MouseEnterExitHandlerObject() noexcept = default;
	};

	#define MOUSE_INPUT_HANDLER_CTOR_DECLARE(className)\
		className(UIDriver& driver) noexcept : className(driver, NULL) { }\
		className(UIDriver& driver, Container* container) noexcept;\

	enum MouseMoveEvent
	{
		Enter,
		Exit,
		Move
	};

	class MouseMoveHandlerObject : public TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseMoveEvent>
	{
	private:
		bool m_isMouseEnterEnabled;
		bool m_isMouseExitEnabled;
		bool m_isMouseMoveOutsideEnabled;
		bool m_isInside;

		// position: is the mouse position in global coordinates
		bool update(Vec2Df position) noexcept;

	protected:
		virtual bool onMouseMove(MouseMoveEvent event, Vec2Df position) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseMoveHandlerObject)

		void enableMouseEnter(bool isEnable) noexcept { m_isMouseEnterEnabled = isEnable; }
		void enableMouseExit(bool isEnable) noexcept { m_isMouseExitEnabled = isEnable; }
		void enableMouseMoveOutside(bool isEnable) noexcept { m_isMouseMoveOutsideEnabled = isEnable; }

		bool isMouseEnterEnabled() const noexcept { return m_isMouseEnterEnabled; }
		bool isMouseExitEnabled() const noexcept { return m_isMouseExitEnabled; }
		bool isMouseMoveOutsideEnabled() const noexcept { return m_isMouseMoveOutsideEnabled; }

		bool isMousePosInside() noexcept;

		// This can be called to recalculate and recheck the mouse pointer if it is inside the rect
		// That also means onMouseEnter() or onMouseExit() may be called.
		// However, (for now) onMouseMove() is not called.
		void update() noexcept;

	public:
		virtual ~MouseMoveHandlerObject() noexcept = default;

		virtual void sleep() noexcept override;
		virtual void awake() noexcept override;
	};

	class MouseClickHandlerObject : public TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseButtonEvent>
	{
	protected:
		virtual bool onMouseClick(MouseButton button, KeyEvent action) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseClickHandlerObject)

	public:
		virtual ~MouseClickHandlerObject() noexcept = default;
	};

	class MouseAnyClickHandlerObject : public TMouseEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>
	{
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action, bool isInside) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseAnyClickHandlerObject)

	public:
		virtual ~MouseAnyClickHandlerObject() noexcept = default;
	};

	class MouseScrollHandlerObject : public TMouseEventHandlerContainerObject<OrderedInputEventsDispatcher::OnMouseScrollEvent>
	{
	protected:
		virtual bool onMouseScroll(Vec2Df scrollDelta) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseScrollHandlerObject)

	public:
		virtual ~MouseScrollHandlerObject() noexcept = default;
	};

	class KeyboardHandlerObject : public TInputEventHandlerObject<IInputDriver::OnKeyboardEvent>
	{
	protected:
		virtual void onKey(KeyCode keycode, KeyEvent action, ModifierKeys modifiers) = 0;

		KeyboardHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~KeyboardHandlerObject() noexcept = default;
	};
}