#pragma once

#include <sutk/defines.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/defines.hpp>

namespace SUTK
{
	class TextGroup;
	class SUTK_API TextGroupContainer : public RenderableContainer
	{
	public:
		TextGroupContainer(UIDriver& driver, Container* parent = NULL, bool isLayoutIgnore = false, Layer layer = InvalidLayer) noexcept;

		TextGroup* getTextGroup() noexcept;
	};
}