#pragma once

#include <sutk/VBoxContainer.hpp>
#include <sutk/Button.hpp>

#include <common/DynamicPool.hpp>

#include <optional> // for std::optional

namespace SUTK
{
	class SUTK_API ButtonListView : public VBoxContainer
	{
	public:
		typedef std::function<void(Button*)> OnPostButtonCreateHandler;
	private:
		GfxDriverObjectHandleType m_textGroup;
		// This must come before m_buttonPool, see: onButtonCreate
		std::optional<OnPostButtonCreateHandler> m_buttonHandler;
		com::DynamicPool<Button*> m_buttonPool;
		f32 m_minWidth;
		f32 m_yOffset;

		// Invoked when a button is returned to the pool
		// Typically, this would be used to deactivate the button to make it invisible and do not consume resources
		void onButtonReturn(Button* &button) noexcept;
		// Invoekd when a button is recycled (re-used) from the pool
		// Typically, this would be used to reactivate the button
		void onButtonRecycle(Button* &button) noexcept;
	protected:
		// Invoked when the pool requests a new button instance
		// Mandatory to be called in the overriding method
		virtual Button* onButtonCreate() noexcept;
	public:
		ButtonListView(UIDriver& driver, Container* parent, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		void setOnPostButtonCreateHandler(OnPostButtonCreateHandler handler) noexcept;

		std::pair<Button*, f32> addButton(const std::string_view labelStr) noexcept;
		// Call this to resize the ButtonListView to fit all the buttons
		// Not calling this might need to visual artefacts
		void fit() noexcept;

		void clear() noexcept;
	};
}