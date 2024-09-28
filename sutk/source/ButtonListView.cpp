#include <sutk/ButtonListView.hpp>

#include <sutk/Button.hpp>
#include <sutk/SmallText.hpp>
#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp>

#include <sutk/ContainerUtility.hpp>

#define MARGIN 1.0f // margin of 1 centimeters

namespace SUTK
{
	ButtonListView::ButtonListView(UIDriver& driver, Container* parent, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : VBoxContainer(driver, parent),
																							m_textGroup(textGroup),
																							m_buttonPool(std::bind(&ButtonListView::onButtonCreate, this),
																										std::bind(&ButtonListView::onButtonReturn, this, std::placeholders::_1),
																										std::bind(&ButtonListView::onButtonRecycle, this, std::placeholders::_1),
																										true,
																										poolSize),
																							m_minWidth(0),
																							m_yOffset(0)
	{
		setRect({ 0.0f, 0.0f, 2.0f, 5.0f });
		m_minWidth = 2.0f;
		setTight(true);
	}

	Button* ButtonListView::onButtonCreate() noexcept
	{
		Button* button = getUIDriver().createContainer<Button>(this, true, m_textGroup);
		button->setRecycleState(Container::RecycleState::Recycled);
		return button;
	}

	void ButtonListView::onButtonReturn(Button* &button) noexcept
	{
		button->getOnPressEvent().clear();
		ContainerUtility::SetActiveAllRecursive(button, false);
		button->setRecycleState(Container::RecycleState::Disposed);
	}

	void ButtonListView::onButtonRecycle(Button* &button) noexcept
	{
		// Order of function call is important here,
		// As the SetActiveAllRecursive() checks of recycled, but before this the button was in disposed state
		// So we need to mark it recycled first so that the button and all of its child contains can be become active
		button->setRecycleState(Container::RecycleState::Recycled);
		ContainerUtility::SetActiveAllRecursive(button, true);
	}

	std::pair<Button*, f32> ButtonListView::addButton(const std::string_view labelStr) noexcept
	{
		f32 yOffset = m_yOffset;
		
		// Create the Interactive Button Object
		Button* button = m_buttonPool.get();
		LayoutAttributes attr = button->getLayoutAttributes();
		attr.minSize.height = 0.7f;
		attr.prefSize.height = 0.7f;
		button->setLayoutAttributes(attr);

		// Setup its Visual Representation
		auto& visualRect = button->getGraphicAs<DefaultButtonGraphic>()->getVisualButtonRect();
		auto& label = button->getGraphicAs<DefaultButtonGraphic>()->getLabel();
		label.set(labelStr);
		label.getText().setFontSize(9);
		visualRect.setIdleColor(Color4::grey(0.05f));
		visualRect.setHoverColor(Color4::grey(0.1f));
		visualRect.setPressColor(Color4::grey(0.15f));
		visualRect.setTransitionDelay(0.08f);

		visualRect.setRoundness(0.0f);
		m_yOffset += attr.prefSize.height;

		Vec2Df boundSize = button->getGraphic()->getMinBoundSize();
		if(m_minWidth < boundSize.width)
			m_minWidth = boundSize.width;

		return { button, yOffset };
	}

	void ButtonListView::fit() noexcept
	{
		setSize({ m_minWidth + MARGIN, getSize().height });
	}

	void ButtonListView::clear() noexcept
	{
		m_buttonPool.reclaim();
		m_yOffset = 0;
	}
}
