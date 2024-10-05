#pragma once

#include <sutk/Button.hpp>
#include <sutk/Types.hpp>

namespace SUTK
{
	class IToggleButtonGraphic;
	class SUTK_API ToggleButton : public Button
	{
	public:
		typedef com::Event<com::no_publish_ptr_t, ToggleState> OnToggleEvent;
	protected:
		virtual void onMouseClick(MouseButton button, KeyEvent action) noexcept override;
	private:
		OnToggleEvent m_onToggleEvent;
		ToggleState m_toggleState;
		IToggleButtonGraphic* m_toggleGraphic;

		void setToggleGraphic(IToggleButtonGraphic* graphic) noexcept;
	public:
		ToggleButton(UIDriver& driver, Container* parent, ToggleState state = ToggleState::Off, bool isCreateDefaultGraphic = true, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		// Override of Button::clearAllEvents()
		virtual void clearAllEvents() noexcept override;

		// Override of Button::setGraphic()
		virtual void setGraphic(IButtonGraphic* graphic) noexcept override;

		IToggleButtonGraphic* getToggleGraphic() noexcept { return m_toggleGraphic; }
		template<typename T> requires (std::derived_from<T, IToggleButtonGraphic>)
		T* getToggleGraphicAs() noexcept { return com::iknow_down_cast<T*>(getToggleGraphic()); }

		void toggle() noexcept;
		void setToggleState(ToggleState state) noexcept;
		ToggleState getToggleState() const noexcept { return m_toggleState; }
	};
}