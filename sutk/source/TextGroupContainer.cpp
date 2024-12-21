#include <sutk/TextGroupContainer.hpp>
#include <sutk/TextGroup.hpp>

namespace SUTK
{
	TextGroupContainer::TextGroupContainer(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : RenderableContainer(driver, parent, isLayoutIgnore, layer), m_textGroup(com::null_pointer<TextGroup>())
	{
		m_textGroup = driver.createRenderable<TextGroup>(this);
	}
	TextGroupContainer::~TextGroupContainer() noexcept
	{
		getUIDriver().destroyRenderable<TextGroup>(m_textGroup);
	}

	GfxDriverObjectHandleType TextGroupContainer::getGfxDriverObjectHandle() noexcept
	{
		return getTextGroup()->getGfxDriverObjectHandle();
	}

 	TextGroup* TextGroupContainer::getTextGroup() noexcept
 	{ 
 		return m_textGroup;
 	}
}