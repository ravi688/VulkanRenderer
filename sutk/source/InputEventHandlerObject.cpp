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

	MouseMoveHandlerObject::MouseMoveHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnMouseMoveEvent>(driver.getInputDriver().getOnCursorMoveEvent())
	{
		IInputDriver::OnMouseMoveEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, SUTK::Vec2Df position)
		{
			onMouseMove(position);
		});
		setSubscriptionID(id);
	}

	MouseClickHandlerObject::MouseClickHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnMouseButtonEvent>(driver.getInputDriver().getOnMouseButtonEvent())
	{
		IInputDriver::OnMouseButtonEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, MouseButton button, KeyEvent event)
		{
			onMouseClick(button, event);
		});
		setSubscriptionID(id);
	}

	MouseScrollHandlerObject::MouseScrollHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnMouseScrollEvent>(driver.getInputDriver().getOnScrollEvent())
	{
		IInputDriver::OnMouseScrollEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, Vec2Df scrollDelta)
		{
			onMouseScroll(scrollDelta);
		});
		setSubscriptionID(id);
	}

	KeyboardHandlerObject::KeyboardHandlerObject(UIDriver& driver) noexcept : TInputEventHandlerObject<IInputDriver::OnKeyboardEvent>(driver.getInputDriver().getOnKeyEvent())
	{
		IInputDriver::OnKeyboardEvent::SubscriptionID id = getEvent().subscribe([this](IInputDriver* inputDriver, KeyCode keycode, KeyEvent action)
		{
			onKey(keycode, action);
		});
		setSubscriptionID(id);
	}	
}