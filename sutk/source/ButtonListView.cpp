#include <sutk/ButtonListView.hpp>

#include <sutk/SmallText.hpp>
#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp>

#include <sutk/ContainerUtility.hpp>
#include <sutk/Constants.hpp>

namespace SUTK
{
	ButtonListView::ButtonListView(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : DynamicVListContainer<Button>(driver, parent, isLayoutIgnore, layer, 0),
																																		m_textGroup(textGroup)
	{
		// The reason we first set the poolSize to zero (in the Constructor), and then call reserve()
		// is that, onCreate() uses m_textGroup, so we must allowed it be initialized first.
		getPool().reserve(poolSize);
	}

	Button* ButtonListView::onCreate() noexcept
	{
		Button* button = getUIDriver().createContainer<Button>(this, /* isCreateDefaultGraphic: */ true, m_textGroup);
		button->setRecycleState(Container::RecycleState::Recycled);
		button->getGraphicAs<DefaultButtonGraphic>()->getLabel().setAlignment(HorizontalAlignment::Left, VerticalAlignment::Middle);
		invokeOnPostCreateHandler(button);
		return button;
	}

	void ButtonListView::onReturn(Button* &button) noexcept
	{
		button->clearAllEvents();
		ContainerUtility::SetActiveAllRecursive(button, com::Bool::False());
		button->setRecycleState(Container::RecycleState::Disposed);
	}

	void ButtonListView::onRecycle(Button* &button) noexcept
	{
		// Order of function call is important here,
		// As the SetActiveAllRecursive() checks of recycled, but before this the button was in disposed state
		// So we need to mark it recycled first so that the button and all of its child contains can be become active
		button->setRecycleState(Container::RecycleState::Recycled);
		ContainerUtility::SetActiveAllRecursive(button, com::Bool::True());
	}

	std::pair<Button*, f32> ButtonListView::addButton(const std::string_view labelStr) noexcept
	{	
		// Create the Interactive Button Object
		std::pair<Button*, f32> result = get(Constants::Defaults::ButtonListView::ButtonHeight);

		// Setup its Visual Representation
		DefaultButtonGraphic* graphic = result.first->getGraphicAs<DefaultButtonGraphic>();
		auto& label = graphic->getLabel();
		label.set(labelStr);
		graphic->getRenderRect().setRoundness(0.0f);

		Vec2Df boundSize = result.first->getGraphic()->getMinBoundSize();
		if(getWidth() < boundSize.width)
			setWidth(boundSize.width);

		return result;
	}
}
