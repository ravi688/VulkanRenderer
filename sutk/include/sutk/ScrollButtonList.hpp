#pragma once

#include <sutk/ButtonListView.hpp>
#include <sutk/MaskedScrollableContainer.hpp>

namespace SUTK
{
	class SUTK_API ScrollButtonList : public MaskedScrollableContainer<ButtonListView>
	{
	protected:
		virtual Button* onButtonCreate() noexcept override;
		virtual void onParentResize(const SUTK::Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		ScrollButtonList(UIDriver& driver, Container* parent, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
	};
}
