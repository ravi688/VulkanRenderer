#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/IInputDriver.hpp> // for SUTK::MouseButton and SUTK::KeyEvent
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
	};

	template<typename InputEventType>
	class TInputEventHandlerContainerObject : public TInputEventHandlerObject<InputEventType>
	{
	private:
		Container* m_container;

	protected:
		TInputEventHandlerContainerObject(InputEventType& event) noexcept : TInputEventHandlerObject<InputEventType>(event), m_container(NULL) { }
		TInputEventHandlerContainerObject(InputEventType& event, Container* container) noexcept : TInputEventHandlerObject<InputEventType>(event), m_container(container)
		{
			if(m_container != NULL)
			{
				// If the container has been deactivated then sleep the subscription,
				// Or if the container has been activated then awake the subscription
				// This saves CPU cycles
				m_container->getOnActiveEvent().subscribe([this](bool isActive)
				{
					if(isActive)
						this->awake();
					else
						this->sleep();
				});
			}
		}
		bool isInside(Vec2Df point) const noexcept
		{
			return (m_container != NULL) ? m_container->containsGlobalCoords(point) : true;
		}
		Container* getContainer() noexcept { return m_container; }
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

	class MouseMoveHandlerObject : public TInputEventHandlerContainerObject<IInputDriver::OnMouseMoveEvent>
	{
	private:
		bool m_isMouseEnterEnabled;
		bool m_isMouseExitEnabled;
		bool m_isInside;
		IInputDriver& m_inputDriver;

		// position: is the mouse position in global coordinates
		void update(Vec2Df position) noexcept;

	protected:
		virtual void onMouseMove(Vec2Df position) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseMoveHandlerObject)

		virtual void onMouseEnter() noexcept { }
		virtual void onMouseExit() noexcept { }

		void enableMouseEnter(bool isEnable) noexcept { m_isMouseEnterEnabled = isEnable; }
		void enableMouseExit(bool isEnable) noexcept { m_isMouseExitEnabled = isEnable; }

		// This can be called to recalculate and recheck the mouse pointer if it is inside the rect
		// That also means onMouseEnter() or onMouseExit() may be called.
		// However, (for now) onMouseMove() is not called.
		void update() noexcept;

	public:
		virtual ~MouseMoveHandlerObject() noexcept = default;

		virtual void sleep() noexcept override;
		virtual void awake() noexcept override;
	};

	class MouseClickHandlerObject : public TInputEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>
	{
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseClickHandlerObject)

	public:
		virtual ~MouseClickHandlerObject() noexcept = default;
	};

	class MouseAnyClickHandlerObject : public TInputEventHandlerContainerObject<IInputDriver::OnMouseButtonEvent>
	{
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action, bool isInside) = 0;

		MOUSE_INPUT_HANDLER_CTOR_DECLARE(MouseAnyClickHandlerObject)

	public:
		virtual ~MouseAnyClickHandlerObject() noexcept = default;
	};

	class MouseScrollHandlerObject : public TInputEventHandlerContainerObject<IInputDriver::OnMouseScrollEvent>
	{
	protected:
		virtual void onMouseScroll(Vec2Df scrollDelta) = 0;

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