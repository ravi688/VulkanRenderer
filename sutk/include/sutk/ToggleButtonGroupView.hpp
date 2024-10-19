#pragma once

#include <sutk/ToggleButtonListView.hpp>

namespace SUTK
{
	class SUTK_API ToggleButtonGroupView : public ToggleButtonListView
	{
	public:
		typedef com::Event<com::no_publish_ptr_t, u32> OnSelectEvent;
	private:
		OnSelectEvent m_onSelectEvent;
	protected:
		// Override of ToggleButtonListView::onCreate()
		virtual ToggleButton* onCreate() noexcept override;
		// Override of ToggleButtonListView::onRecycle()
		virtual void onRecycle(ToggleButton* &button) noexcept override;
	public:
		ToggleButtonGroupView(UIDriver& driver, Container* parent, bool isLayoutIgnore = false, Layer layer = InvalidLayer, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		void select(u32 index) noexcept;

		OnSelectEvent& getOnSelectEvent() noexcept { return m_onSelectEvent; }
	};
}