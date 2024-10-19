#include <sutk/ToggleButtonGraphic.hpp>
#include <sutk/RenderRectFillCircle.hpp>
#include <sutk/Label.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/ContainerUtility.hpp>

#define CIRCLE_THINKNESS 0.1f
#define CIRCLE_TOP_BOTTOM_MARGIN 0.2f
#define CIRCLE_LEFT_MARGIN 0.2f

#define LABEL_MARGIN 0.25f

namespace SUTK
{
	DefaultToggleButtonGraphic::DefaultToggleButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : DefaultButtonGraphic(driver, parent, textGroup)
	{
		m_renderRectCont = driver.createContainer<RenderableContainer>(this);
		m_renderRectCont->setRecycleState(Container::RecycleState::Recycled);
		f32 diameter = getSize().height - CIRCLE_TOP_BOTTOM_MARGIN * 2;
		m_renderRectCont->setRect({ { CIRCLE_LEFT_MARGIN, CIRCLE_TOP_BOTTOM_MARGIN }, { diameter, diameter } });
		m_renderRectCont->getAnchorRect()->setRect({ 0.0f, 0.5f, 0.0f, 0.0f });
		m_renderRect = driver.createRenderable<RenderRectFillCircle>(m_renderRectCont);
		m_renderRect->setSharpness(4.0f);
		m_renderRect->setNumGradPixels(2.0f);
		auto& label = getLabel();
		label.fitInParent({ CIRCLE_LEFT_MARGIN + diameter + LABEL_MARGIN, 0.0f, 0.0f, 0.0f });
		label.getText().setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
	}
	void DefaultToggleButtonGraphic::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		DefaultButtonGraphic::onResize(newRect, isPositionChanged, isSizeChanged);
		if(isSizeChanged)
		{
			f32 diameter = getSize().height - CIRCLE_TOP_BOTTOM_MARGIN * 2;
			m_renderRectCont->setRect({ { CIRCLE_LEFT_MARGIN, CIRCLE_TOP_BOTTOM_MARGIN }, { diameter, diameter } });
			getLabel().fitInParent({ CIRCLE_LEFT_MARGIN + diameter + LABEL_MARGIN, 0.0f, 0.0f, 0.0f });
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