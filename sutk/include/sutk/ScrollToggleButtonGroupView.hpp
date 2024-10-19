#pragma once

#include <sutk/ToggleButtonGroupView.hpp>
#include <sutk/MaskedScrollableContainer.hpp>

#include <common/Bool.hpp> // for com::Bool

namespace SUTK
{
	class SUTK_API ScrollToggleButtonGroupView : public MaskedScrollableContainer<ToggleButtonGroupView>
	{
	protected:
		virtual ToggleButton* onCreate() noexcept override;
		virtual void onParentResize(const SUTK::Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		ScrollToggleButtonGroupView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		void bringToView(u32 index) noexcept;
	};
}
