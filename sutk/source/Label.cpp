#include <sutk/Label.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver::createRenderable etc.
#include <sutk/SmallText.hpp> // for SUTK::SmallText::setData()
#include <sutk/TextGroupContainer.hpp> // for SUTK::TextGroupContainer::getTextGroup()
#include <sutk/TextGroup.hpp> // for SUTK::TextGroup::getGfxDriverObjectHandle()

namespace SUTK
{
	Label::Label(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : RenderableContainer(driver, parent)
	{
		m_text = driver.createRenderable<SmallText>(this, (textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE) ? driver.getGlobalTextGroup() : textGroup);
		m_text->setAlignment(HorizontalAlignment::Left, VerticalAlignment::Top);
		set("New Label");
	}

	Label::Label(UIDriver& driver, TextGroupContainer* textGroupParent) noexcept : Label(driver, textGroupParent, textGroupParent->getTextGroup()->getGfxDriverObjectHandle())
	{

	}

	void Label::set(const std::string_view str) noexcept
	{
		m_text->setData(str);
	}

	void Label::setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept
	{
		m_text->setAlignment(hAlign, vAlign);
	}
}