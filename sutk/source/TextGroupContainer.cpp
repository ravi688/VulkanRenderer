#include <sutk/TextGroupContainer.hpp>
#include <sutk/TextGroup.hpp>

namespace SUTK
{
	TextGroupContainer::TextGroupContainer(UIDriver& driver, Container* parent, bool isLayoutIgnore, Layer layer) noexcept : RenderableContainer(driver, parent, isLayoutIgnore, layer)
	{
		TextGroup* textGroup = driver.createRenderable<TextGroup>(this);
		setRenderable(textGroup);
	}

 	TextGroup* TextGroupContainer::getTextGroup() noexcept
 	{ 
 		return com::iknow_down_cast<TextGroup*>(getRenderable()); 
 	}
}