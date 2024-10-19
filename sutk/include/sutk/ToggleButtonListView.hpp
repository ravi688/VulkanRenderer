#pragma once

#include <sutk/DynamicVListContainer.hpp>
#include <sutk/ToggleButton.hpp>

namespace SUTK
{
	class SUTK_API ToggleButtonListView : public DynamicVListContainer<ToggleButton>
	{
	public:
		static constexpr f32 TOGGLE_BUTTON_HEIGHT = 0.7f;
	private:
		GfxDriverObjectHandleType m_textGroup;

	protected:
		// Invoked when a button is returned to the pool
		// Typically, this would be used to deactivate the button to make it invisible and do not consume resources
		virtual void onReturn(ToggleButton* &button) noexcept override;
		// Invoekd when a button is recycled (re-used) from the pool
		// Typically, this would be used to reactivate the button
		virtual void onRecycle(ToggleButton* &button) noexcept override;
		// Invoked when the pool requests a new button instance
		// Mandatory to be called in the overriding method
		virtual ToggleButton* onCreate() noexcept override;
	public:
		ToggleButtonListView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		std::pair<ToggleButton*, f32> addButton(const std::string_view labelStr) noexcept;
	};
}