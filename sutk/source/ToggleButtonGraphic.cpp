#include <sutk/ToggleButtonGraphic.hpp>
#include <sutk/RenderRectFillCircle.hpp>
#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/ContainerUtility.hpp>
#include <sutk/Constants.hpp>

namespace SUTK
{
	DefaultToggleButtonGraphic::DefaultToggleButtonGraphic(UIDriver& driver, Container* parent, std::optional<GfxDriverObjectHandleType> textGroup) noexcept : DefaultButtonGraphic(driver, parent, textGroup)
	{
		m_renderRectCont = driver.createContainer<RenderableContainer>(this);
		m_renderRectCont->setRecycleState(Container::RecycleState::Recycled);
		f32 diameter = getSize().height - Constants::Defaults::ToggleButton::CircleTopBottomMargin * 2;
		m_renderRectCont->setRect({ { Constants::Defaults::Button::LeftMargin, Constants::Defaults::ToggleButton::CircleTopBottomMargin }, { diameter, diameter } });
		m_renderRectCont->getAnchorRect()->setRect({ 0.0f, 0.5f, 0.0f, 0.0f });
		m_renderRect = driver.createRenderable<RenderRectFillCircle>(m_renderRectCont);
		m_renderRect->setSharpness(4.0f);
		m_renderRect->setNumGradPixels(2.0f);
		m_renderRect->setColor(Color4::blue());
		auto& label = getLabel();
		label.fitInParent({ Constants::Defaults::Button::LeftMargin + diameter + Constants::Defaults::ToggleButton::CircleLabelSpacing, Constants::Defaults::Button::RightMargin, 0.0f, 0.0f });
		label.getText().setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
		label.set("New Toggle");
	}
	void DefaultToggleButtonGraphic::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		DefaultButtonGraphic::onResize(newRect, isPositionChanged, isSizeChanged);
		if(isSizeChanged)
		{
			f32 diameter = getSize().height - Constants::Defaults::ToggleButton::CircleTopBottomMargin * 2;
			m_renderRectCont->setRect({ { Constants::Defaults::Button::LeftMargin, Constants::Defaults::ToggleButton::CircleTopBottomMargin }, { diameter, diameter } });
			getLabel().fitInParent({ Constants::Defaults::Button::LeftMargin + diameter + Constants::Defaults::ToggleButton::CircleLabelSpacing, Constants::Defaults::Button::RightMargin, 0.0f, 0.0f });
		}
	}

	void DefaultToggleButtonGraphic::onToggle(ToggleState state)
	{
		com::Bool isActive(state != ToggleState::Off);
		if(isActive)
			m_renderRectCont->setRecycleState(Container::RecycleState::Recycled);
		ContainerUtility::SetActiveAllRecursive(m_renderRectCont, isActive);
		if(!isActive)
			m_renderRectCont->setRecycleState(Container::RecycleState::Disposed);
	}

}