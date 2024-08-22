#include <sutk/Label.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver::createRenderable etc.
#include <sutk/SmallText.hpp> // for SUTK::SmallText::setData()

namespace SUTK
{
	Label::Label(UIDriver& driver, Container* parent) noexcept : RenderableContainer(driver, parent)
	{
		m_text = driver.createRenderable<SmallText>(this, driver.getGlobalTextGroup());
		m_text->setAlignment(HorizontalAlignment::Left, VerticalAlignment::Top);
		set("New Label");
	}

	void Label::set(const std::string& str) noexcept
	{
		m_text->setData(str);
	}

	void Label::setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept
	{
		m_text->setAlignment(hAlign, vAlign);
	}
}