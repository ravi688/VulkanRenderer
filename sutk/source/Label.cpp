#include <sutk/Label.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver::createRenderable etc.
#include <sutk/SmallText.hpp> // for SUTK::SmallText::setData()
#include <sutk/TextGroupContainer.hpp> // for SUTK::TextGroupContainer::getTextGroup()
#include <sutk/TextGroup.hpp> // for SUTK::TextGroup::getGfxDriverObjectHandle()
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility::findTextGroupHandle()

namespace SUTK
{
	Label::Label(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : RenderableContainer(driver, parent), 
																										m_text(com::null_pointer<SmallText>()),
																										m_hAlign(HorizontalAlignment::Left),
																										m_vAlign(VerticalAlignment::Top),
																										m_textGroup((textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE) ? getUIDriver().getGlobalTextGroup() : textGroup),
																										m_isReassociateOnParentChange(com::True)
	{

	}

	Label::Label(UIDriver& driver, Container* parent) noexcept : Label(driver, parent, ContainerUtility::findTextGroupHandle(parent))
	{

	}

	Label::~Label() noexcept
	{
		if(m_text)
		{
			_com_assert(m_textGroup != GFX_DRIVER_OBJECT_NULL_HANDLE);
			getUIDriver().destroyRenderable<SmallText>(m_text);
		}
	}

	void Label::onAnscestorChange(Container* _) noexcept
	{
		RenderableContainer::onAnscestorChange(_);
		if(m_isReassociateOnParentChange)
			tryReassociate();
		if(!m_text)
		{
			GfxDriverObjectHandleType textGroup = ContainerUtility::findTextGroupHandle(getParent());
			m_textGroup = (textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE) ? getUIDriver().getGlobalTextGroup() : textGroup;
		}
	}

	void Label::tryReassociate() noexcept
	{
		com_assert_wrn(COM_DESCRIPTION(false), "tryReassociate should not be called for now as the feature hasn't been fully implemented yet");
		GfxDriverObjectHandleType textGroup = ContainerUtility::findTextGroupHandle(getParent());
		textGroup = (textGroup == GFX_DRIVER_OBJECT_NULL_HANDLE) ? getUIDriver().getGlobalTextGroup() : textGroup;
		if(m_text && (textGroup != m_textGroup))
		{
			// TODO:
			// Create a new one
			//	text = getUIDriver().createRenderable<SmallText>(this, textGroup);
			//	text->setAlignment(m_text->getHorizontalAlign(), m_text->getVerticalAlign());
			//	text->setData(m_text->getData());
			// Destroy the old SmallText renderable
			//	getUIDriver().destroyRenderable<SmallText>(m_text);
			//	m_text = text;
			m_textGroup = textGroup;
		}
	}

	void Label::createTextFirstTime() noexcept
	{
		m_text = getUIDriver().createRenderable<SmallText>(this, m_textGroup);
		m_text->setAlignment(m_hAlign, m_vAlign);
		m_text->setData("New Label");
	}

	void Label::setReassociateOnParentChange(com::Bool isReassociate, com::Bool isApplyNow) noexcept
	{
		m_isReassociateOnParentChange = isReassociate;
		if(m_isReassociateOnParentChange && isApplyNow)
			tryReassociate();
	}

	void Label::set(const std::string_view str) noexcept
	{
		if(!m_text)
			createTextFirstTime();
		m_text->setData(str);
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

	SmallText& Label::getText() noexcept
	{
		if(!m_text)
			createTextFirstTime();
		return *m_text;
	}
}