#pragma once

#include <sutk/ToggleButtonGroupView.hpp>
#include <sutk/MaskedScrollableContainer.hpp>

namespace SUTK
{
	class SUTK_API ScrollToggleButtonGroupView : public MaskedScrollableContainer<ToggleButtonGroupView>
	{
	protected:
		virtual ToggleButton* onCreate() noexcept override;
		virtual void onParentResize(const SUTK::Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		ScrollToggleButtonGroupView(UIDriver& driver, Container* parent, bool isLayoutIgnore = false, UIDriver::Layer layer = UIDriver::InvalidLayer, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		void bringToView(u32 index) noexcept;
	};
}
