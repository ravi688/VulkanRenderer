#pragma once

#include <sutk/defines.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/defines.hpp>

namespace SUTK
{
	class TextGroup;
	class SUTK_API TextGroupContainer : public RenderableContainer
	{
	private:
		TextGroup* m_textGroup;
	public:
		TextGroupContainer(UIDriver& driver, Container* parent = NULL, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer) noexcept;

		GfxDriverObjectHandleType getGfxDriverObjectHandle() noexcept;
		TextGroup* getTextGroup() noexcept;
	};
}