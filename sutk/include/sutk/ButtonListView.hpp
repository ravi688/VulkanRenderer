#pragma once

#include <sutk/Button.hpp>
#include <sutk/DynamicVListContainer.hpp>

namespace SUTK
{
	class SUTK_API ButtonListView : public DynamicVListContainer<Button>
	{
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
		// Invoked for every created object (with onCreate()) when the DynamicVListContainer is destroyed
		// Mandatory to be called in the overriding method
		virtual void onDestroy(Button* &button) noexcept override;
	public:
		ButtonListView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
		~ButtonListView() noexcept;

		std::pair<Button*, f32> addButton(const std::string_view labelStr) noexcept;
	};
}