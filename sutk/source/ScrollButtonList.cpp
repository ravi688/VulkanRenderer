#include <sutk/ScrollButtonList.hpp>

namespace SUTK
{
	ScrollButtonList::ScrollButtonList(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : MaskedScrollableContainer<ButtonListView>(driver, parent, isLayoutIgnore, layer, poolSize, textGroup)
	{

	}

	Button* ScrollButtonList::onCreate() noexcept
	{
		auto button = MaskedScrollableContainer<ButtonListView>::onCreate();
		updateMaskFor(button);
		return button;
	}

	void ScrollButtonList::onParentResize(const SUTK::Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		MaskedScrollableContainer<ButtonListView>::onParentResize(newRect, isPositionChanged, isSizeChanged);
		setSize({ newRect.width - getScrollContainer()->getScrollDelta().x, getSize().height });
	}
}