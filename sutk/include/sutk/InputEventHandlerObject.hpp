#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/IInputDriver.hpp> // for SUTK::MouseButton and SUTK::KeyEvent

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
		virtual void sleep() noexcept override
		{
			_com_assert(m_id != InputEventType::InvalidSubscriptionID);
			m_event.deactivate(m_id);
		}

		virtual void awake() noexcept override
		{
			_com_assert(m_id != InputEventType::InvalidSubscriptionID);
			m_event.activate(m_id);
		}
	};

	class MouseEnterExitHandlerObject : public TInputEventHandlerObject<IInputDriver::OnMouseEnterExitEvent>
	{
	protected:
		virtual void onMouseEnter() = 0;
		virtual void onMouseExit() = 0;

		MouseEnterExitHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~MouseEnterExitHandlerObject() noexcept = default;
	};

	class MouseMoveHandlerObject : public TInputEventHandlerObject<IInputDriver::OnMouseMoveEvent>
	{
	protected:
		virtual void onMouseMove(Vec2Df position) = 0;

		MouseMoveHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~MouseMoveHandlerObject() noexcept = default;
	};

	class MouseClickHandlerObject : public TInputEventHandlerObject<IInputDriver::OnMouseButtonEvent>
	{
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action) = 0;

		MouseClickHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~MouseClickHandlerObject() noexcept = default;
	};

	class MouseScrollHandlerObject : public TInputEventHandlerObject<IInputDriver::OnMouseScrollEvent>
	{
	protected:
		virtual void onMouseScroll(Vec2Df scrollDelta) = 0;

		MouseScrollHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~MouseScrollHandlerObject() noexcept = default;
	};

	class KeyboardHandlerObject : public TInputEventHandlerObject<IInputDriver::OnKeyboardEvent>
	{
	protected:
		virtual void onKey(KeyCode keycode, KeyEvent action) = 0;

		KeyboardHandlerObject(UIDriver& driver) noexcept;

	public:
		virtual ~KeyboardHandlerObject() noexcept = default;
	};
}