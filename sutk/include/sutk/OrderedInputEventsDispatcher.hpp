#pragma once

#include <sutk/defines.hpp> // for SUTK_API
#include <sutk/IInputDriver.hpp> // for SUTK::IInputDriver
#include <common/OrderedEvent.hpp> // for com::OrderedEvent

namespace SUTK
{
	class SUTK_API OrderedInputEventsDispatcher
	{
	public:
		typedef com::OrderedEvent<IInputDriver, u32, std::greater<u32>, Vec2Df> OnMouseMoveEvent;
		typedef com::OrderedEvent<IInputDriver, u32, std::greater<u32>, MouseButton, KeyEvent> OnMouseButtonEvent;
		typedef com::OrderedEvent<IInputDriver, u32, std::greater<u32>, Vec2Df> OnMouseScrollEvent;
		typedef com::OrderedEvent<IInputDriver, u32, std::greater<u32>, KeyCode, KeyEvent, ModifierKeys> OnKeyboardEvent;
		typedef com::OrderedEvent<IInputDriver, u32, std::greater<u32>, bool> OnMouseEnterExitEvent;
	private:
		IInputDriver& m_inputDriver;
		OnMouseMoveEvent m_onCursorMoveEvent;
		OnMouseButtonEvent m_onMouseButtonEvent;
		OnMouseScrollEvent m_onScrollEvent;
		OnKeyboardEvent m_onKeyEvent;
		OnMouseEnterExitEvent m_onMouseEnterExitEvent;
		OnMouseMoveEvent::SubscriptionID m_OnCursorMoveEventID;
		OnMouseButtonEvent::SubscriptionID m_OnMouseButtonEventID;
		OnMouseScrollEvent::SubscriptionID m_OnScrollEventID;
		OnKeyboardEvent::SubscriptionID m_OnKeyEventID;
		OnMouseEnterExitEvent::SubscriptionID m_OnMouseEnterExitEventID;
	public:
		OrderedInputEventsDispatcher(IInputDriver& inputDriver) noexcept;
		~OrderedInputEventsDispatcher() noexcept;

		OnMouseMoveEvent& getOnCursorMoveEvent() { return m_onCursorMoveEvent; }
		OnMouseButtonEvent& getOnMouseButtonEvent() { return m_onMouseButtonEvent; }
		OnMouseScrollEvent& getOnScrollEvent() { return m_onScrollEvent; }
		OnKeyboardEvent& getOnKeyEvent() { return m_onKeyEvent; }
		OnMouseEnterExitEvent& getOnMouseEnterExitEvent() { return m_onMouseEnterExitEvent; }
	};
}