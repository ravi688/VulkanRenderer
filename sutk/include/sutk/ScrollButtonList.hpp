#pragma once

#include <sutk/ButtonListView.hpp>
#include <sutk/MaskedScrollableContainer.hpp>

namespace SUTK
{
	class SUTK_API ScrollButtonList : public MaskedScrollableContainer<ButtonListView>
	{
	protected:
		virtual Button* onButtonCreate() noexcept override;
	public:
		ScrollButtonList(UIDriver& driver, Container* parent, u32 poolSize = 7, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
	};
}
