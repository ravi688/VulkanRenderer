#pragma once

#include <sutk/defines.hpp>
#include <sutk/Renderable.hpp>

namespace SUTK
{
	class SUTK_API TextGroup : public GfxDriverRenderable
	{
	public:
		TextGroup(UIDriver& driver, RenderableContainer* container = NULL) noexcept;

		// Implementation of GfxDriverRenderable
		virtual bool isDirty() override { return false; }
		virtual void update() override { }
	};
}
