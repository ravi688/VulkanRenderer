#include <sutk/Label.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver::createRenderable etc.
#include <sutk/SmallText.hpp> // for SUTK::SmallText::setData()

namespace SUTK
{
	Label::Label(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : RenderableContainer(driver, parent), 
																										m_text(com::null_pointer<SmallText>()),
																										m_hAlign(HorizontalAlignment::Left),
																										m_vAlign(VerticalAlignment::Top)
	{
		setTextGroup(textGroup);
	}

	Label::~Label() noexcept
	{
		if(m_text)
			getUIDriver().destroyRenderable<SmallText>(m_text);
		if(m_ownedTextGroup != GFX_DRIVER_OBJECT_NULL_HANDLE)
			getGfxDriver().destroyTextGroup(m_ownedTextGroup);
	}

	void Label::createTextFirstTime() noexcept
	{
		setTextGroupForce(m_textGroup);
		m_text = getUIDriver().createRenderable<SmallText>(this, m_textGroup);
		m_text->setAlignment(m_hAlign, m_vAlign);
		m_text->setData("New Label");
	}

	void Label::setTextGroupForce(GfxDriverObjectHandleType textGroup) noexcept
	{
		com::Bool isDifferent { textGroup != m_textGroup };
		// If the provided Text Group is invalid then create an owned Text Group if not already, and use that one
		if(textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE)
		{
			if(m_ownedTextGroup == GFX_DRIVER_OBJECT_NULL_HANDLE)
				m_ownedTextGroup = getGfxDriver().createTextGroup(RenderMode::Transparent);
			textGroup = m_ownedTextGroup;
		}
		// Recreate SmallText if new text group doesn't match with old text group
		if(isDifferent)
		{
			debug_log_info("Recreating Small Text due to reassociation");

			// Save clonable data before destroying the m_text renderable (SmallText)
			auto hAlign = m_text->getHorizontalAlignment();
			auto vAlign = m_text->getVerticalAlignment();
			auto data = m_text->getData();
			auto color = m_text->getColor();
			auto fontSize = m_text->getFontSize();
			auto isActive = m_text->isActive();
			auto font = m_text->getFont();

			// Destroy the old SmallText renderable first as we can't associate a new Renderable to a RenderableContainer
			// already associated with its old Renderable (SmallText)
			getUIDriver().destroyRenderable<SmallText>(m_text);

			// Create a new SmallText
			SmallText* text = getUIDriver().createRenderable<SmallText>(this, textGroup);
			text->setAlignment(hAlign, vAlign);
			text->setData(data);
			text->setColor(color);
			text->setFontSize(fontSize);
			text->setActive(isActive);
			text->setFont(font);

			m_text = text;
		}
		m_textGroup = textGroup;
	}

	SmallText& Label::getText() noexcept
	{
		if(!m_text)
			createTextFirstTime();
		return *m_text;
	}

	void Label::setTextGroup(GfxDriverObjectHandleType textGroup) noexcept
	{
		// If no SmallText object has already been created then we don't need to instantiate any objects, in the favor of lazy instantiation
		if(!m_text)
		{
			m_textGroup = textGroup;
			return;
		}
		setTextGroupForce(textGroup);
	}

	void Label::set(const std::string_view str) noexcept
	{
		if(!m_text)
			createTextFirstTime();
		m_text->setData(str);
	}

	const std::string& Label::get() noexcept
	{
		if(!m_text)
			createTextFirstTime();
		const std::string& str = m_text->getData();
		return str;
	}

	void Label::setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept
	{
		if(!m_text)
		{
			m_hAlign = hAlign;
			m_vAlign = vAlign;
			return;
		}
		m_text->setAlignment(hAlign, vAlign);
	}

	Renderable* Label::getRenderable() noexcept
	{
		return &getText();
	}

	f32 Label::getHorizontalBound() const noexcept
	{
		return const_cast<Label*>(this)->getText().getCoordFromColPos(END_OF_LINE);
	}

	void Label::setColor(Color4 color) noexcept
	{
		getText().setColor(color);
	}

	Color4 Label::getColor() const noexcept
	{
		return const_cast<Label*>(this)->getText().getColor();
	}

	void Label::setFont(UIDriver::FontReference font) noexcept
	{
		getText().setFont(font);
	}

	void Label::setFontSize(f32 fontSize) noexcept
	{
		getText().setFontSize(fontSize);
	}
}