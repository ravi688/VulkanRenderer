#include <sutk/ScrollButtonList.hpp>

namespace SUTK
{
	ScrollButtonList::ScrollButtonList(UIDriver& driver, Container* parent, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : MaskedScrollableContainer<ButtonListView>(driver, parent, poolSize, textGroup)
	{

	}

	Button* ScrollButtonList::onButtonCreate() noexcept
	{
		auto button = MaskedScrollableContainer<ButtonListView>::onButtonCreate();
		updateMaskFor(button);
		return button;
	}
}