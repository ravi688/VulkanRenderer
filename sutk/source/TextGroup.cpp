#include <sutk/TextGroup.hpp>

namespace SUTK
{
	TextGroup::TextGroup(UIDriver& driver, RenderableContainer* container) noexcept : GfxDriverRenderable(driver, container)
	{
		setGfxDriverObjectHandle(getGfxDriver().createTextGroup(RenderMode::Transparent));
	}
}