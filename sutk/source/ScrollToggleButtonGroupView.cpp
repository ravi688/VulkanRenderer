#include <sutk/ScrollToggleButtonGroupView.hpp>

#include <common/debug.hpp>

namespace SUTK
{
	ScrollToggleButtonGroupView::ScrollToggleButtonGroupView(UIDriver& driver, Container* parent, bool isLayoutIgnore, Layer layer, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : MaskedScrollableContainer<ToggleButtonGroupView>(driver, parent, isLayoutIgnore, layer, poolSize, textGroup)
	{
		getScrollContainer()->setScaleFactor(TOGGLE_BUTTON_HEIGHT);
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

	void ScrollToggleButtonGroupView::bringToView(u32 index) noexcept
	{
		auto activeCount = getPool().activeCount();
		if(index > activeCount)
		{
			DEBUG_LOG_ERROR("Index (%lu) exceeds the active count (%lu)", index, activeCount);
			return;
		}
		SUTK::ScrollContainer* scrollContainer = getScrollContainer();
		f32 scroll = scrollContainer->getSize().height - (index + 1) * TOGGLE_BUTTON_HEIGHT - scrollContainer->getScrollDelta().y;
		scrollContainer->addScrollDelta({ 0, scroll });
	}
}