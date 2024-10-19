#include <sutk/ToggleButtonListView.hpp>

#include <sutk/SmallText.hpp>
#include <sutk/ToggleButtonGraphic.hpp>
#include <sutk/Label.hpp>

#include <sutk/ContainerUtility.hpp>

namespace SUTK
{
	ToggleButtonListView::ToggleButtonListView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : DynamicVListContainer<ToggleButton>(driver, parent, isLayoutIgnore, layer, 0),
																																		m_textGroup(textGroup)
	{
		lockLayout();
		// The reason we first set the poolSize to zero (in the Constructor), and then call reserve()
		// is that, onCreate() uses m_textGroup, so we must allowed it be initialized first.
		getPool().reserve(poolSize);
		unlockLayout(true);
	}

	ToggleButton* ToggleButtonListView::onCreate() noexcept
	{
		ToggleButton* button = getUIDriver().createContainer<ToggleButton>(this, ToggleState::Off, true, m_textGroup);
		button->setRecycleState(Container::RecycleState::Recycled);
		invokeOnPostCreateHandler(button);
		return button;
	}

	void ToggleButtonListView::onReturn(ToggleButton* &button) noexcept
	{
		button->clearAllEvents();
		ContainerUtility::SetActiveAllRecursive(button, com::Bool::False());
		button->setRecycleState(Container::RecycleState::Disposed);
	}

	void ToggleButtonListView::onRecycle(ToggleButton* &button) noexcept
	{
		// Order of function call is important here,
		// As the SetActiveAllRecursive() checks of recycled, but before this the button was in disposed state
		// So we need to mark it recycled first so that the button and all of its child contains can be become active
		button->setRecycleState(Container::RecycleState::Recycled);
		ContainerUtility::SetActiveAllRecursive(button, com::Bool::True());
	}

	std::pair<ToggleButton*, f32> ToggleButtonListView::addButton(const std::string_view labelStr) noexcept
	{	
		// Create the Interactive ToggleButton Object
		std::pair<ToggleButton*, f32> result = get(TOGGLE_BUTTON_HEIGHT);

		// Setup its Visual Representation
		DefaultToggleButtonGraphic* graphic = result.first->getGraphicAs<DefaultToggleButtonGraphic>();
		auto& label = graphic->getLabel();
		label.set(labelStr);
		label.getText().setFontSize(9);
		graphic->setIdleColor(Color4::grey(0.05f));
		graphic->setHoverColor(Color4::grey(0.1f));
		graphic->setPressColor(Color4::grey(0.15f));
		graphic->setTransitionDelay(0.08f);
		graphic->getRenderRect().setRoundness(0.0f);

		Vec2Df boundSize = result.first->getGraphic()->getMinBoundSize();
		if(getWidth() < boundSize.width)
			setWidth(boundSize.width);

		return result;
	}
}
