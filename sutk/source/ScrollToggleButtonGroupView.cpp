#include <sutk/ScrollToggleButtonGroupView.hpp>

namespace SUTK
{
	ScrollToggleButtonGroupView::ScrollToggleButtonGroupView(UIDriver& driver, Container* parent, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : MaskedScrollableContainer<ToggleButtonGroupView>(driver, parent, poolSize, textGroup)
	{

	}

	ToggleButton* ScrollToggleButtonGroupView::onCreate() noexcept
	{
		auto button = MaskedScrollableContainer<ToggleButtonGroupView>::onCreate();
		updateMaskFor(button);
		return button;
	}

	void ScrollToggleButtonGroupView::onParentResize(const SUTK::Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		MaskedScrollableContainer<ToggleButtonGroupView>::onParentResize(newRect, isPositionChanged, isSizeChanged);
		setSize({ newRect.width - getScrollContainer()->getScrollDelta().x, getSize().height });
	}
}