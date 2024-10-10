#pragma once

#include <sutk/Button.hpp>
#include <sutk/DynamicVListContainer.hpp>

namespace SUTK
{
	class SUTK_API ButtonListView : public DynamicVListContainer<Button>
	{
	public:
		static constexpr f32 BUTTON_HEIGHT = 0.7f;
	private:
		GfxDriverObjectHandleType m_textGroup;

	protected:
		// Invoked when a button is returned to the pool
		// Typically, this would be used to deactivate the button to make it invisible and do not consume resources
		virtual void onReturn(Button* &button) noexcept override;
		// Invoekd when a button is recycled (re-used) from the pool
		// Typically, this would be used to reactivate the button
		virtual void onRecycle(Button* &button) noexcept override;
		// Invoked when the pool requests a new button instance
		// Mandatory to be called in the overriding method
		virtual Button* onCreate() noexcept override;
	public:
		ButtonListView(UIDriver& driver, Container* parent, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		std::pair<Button*, f32> addButton(const std::string_view labelStr) noexcept;
	};
}