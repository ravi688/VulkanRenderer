#include <sutk/TabButtonGraphic.hpp>
#include <sutk/Constants.hpp>
#include <sutk/Label.hpp>
#include <sutk/Button.hpp>

namespace SUTK
{
	UIDriver::ImageReference TabButtonGraphic::s_closeIcon = UIDriver::InvalidImage;

	TabButtonGraphic::TabButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : DefaultButtonGraphicNoLabel(driver, parent)
	{
		m_label = driver.createContainer<Label>(this, textGroup);
		m_label->setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
		m_label->set("New Button");
		m_label->setColor(Color4::black());
		m_label->alwaysFitInParent({ Constants::Defaults::Notebook::Tab::LeftMargin, 
							Constants::Defaults::Notebook::Tab::LabelCloseSpacing 
							+ Constants::Defaults::Notebook::Tab::CloseButtonSize
							+ Constants::Defaults::Notebook::Tab::RightMargin, 0, 0  });

		m_closeButton = driver.createContainer<Button>(this, /* isCreateDefaultGraphic: */ false);
		m_closeButton->setRect({ getSize().width - Constants::Defaults::Notebook::Tab::CloseButtonSize - Constants::Defaults::Notebook::Tab::RightMargin, 
								(getSize().height - Constants::Defaults::Notebook::Tab::CloseButtonSize) * 0.5f,
								 Constants::Defaults::Notebook::Tab::CloseButtonSize,
								 Constants::Defaults::Notebook::Tab::CloseButtonSize });
		m_closeButton->getAnchorRect()->moveToRightMiddleOfParent();
		m_closeButtonGraphic = driver.createContainer<ImageButtonGraphic>(m_closeButton);
		if(!s_closeIcon)
			s_closeIcon = driver.loadImage("svg_files/close-square-svgrepo-com.svg");
		m_closeButtonGraphic->setImage(s_closeIcon);
		
		m_hoverColor = getHoverColor();
		m_pressColor = getPressColor();
		m_idleColor = getIdleColor();
	}

	TabButtonGraphic::~TabButtonGraphic() noexcept
	{
		auto& driver = getUIDriver();
		driver.destroyContainer<ImageButtonGraphic>(m_closeButtonGraphic);
		driver.destroyContainer<Button>(m_closeButton);
		driver.destroyContainer<Label>(m_label);
	}

	Vec2Df TabButtonGraphic::getMinBoundSize() noexcept
	{
		return { m_label->getHorizontalBound() 
				+ Constants::Defaults::Notebook::Tab::CloseButtonSize
				+ Constants::Defaults::Notebook::Tab::LabelCloseSpacing
				+ Constants::Defaults::Notebook::Tab::LeftMargin 
				+ Constants::Defaults::Notebook::Tab::RightMargin, getSize().height };
	}

	void TabButtonGraphic::unselectedState() noexcept
	{
		setHoverColor(m_hoverColor);
		setPressColor(m_pressColor);
		setIdleColor(m_idleColor);
	}

	void TabButtonGraphic::selectedState() noexcept
	{
		setIdleColor(Constants::Defaults::Notebook::Tab::SelectColor);
		setHoverColor(Constants::Defaults::Notebook::Tab::SelectColor);
		setPressColor(Constants::Defaults::Notebook::Tab::SelectColor);
	}
}
