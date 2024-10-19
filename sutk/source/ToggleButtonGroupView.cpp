#include <sutk/ToggleButtonGroupView.hpp>

#include <common/debug.h> // for DEBUG_LOG_ERROR()

namespace SUTK
{
	ToggleButtonGroupView::ToggleButtonGroupView(UIDriver& driver, Container* parent, bool isLayoutIgnore, Layer layer, u32 poolSize, GfxDriverObjectHandleType textGroup) noexcept : ToggleButtonListView(driver, parent, isLayoutIgnore, layer, poolSize, textGroup)
	{

	}

	void ToggleButtonGroupView::select(u32 index) noexcept
	{
		auto& pool = getPool();
		if(index >= pool.activeCount())
		{
			DEBUG_LOG_ERROR("Unable to select, Invalid index %u", index);
			return;
		}

		auto it = std::next(pool.getActives().begin(), index);
		(*it)->setToggleState(ToggleState::On);
	}

	ToggleButton* ToggleButtonGroupView::onCreate() noexcept
	{
		ToggleButton* button = ToggleButtonListView::onCreate();
		button->setOneWayToggle(true);
		return button;
	}

	void ToggleButtonGroupView::onRecycle(ToggleButton* &button) noexcept
	{
		ToggleButtonListView::onRecycle(button);
		// NOTE: activeCount() returns one less than the current actives (including this)
		// That also means, DynamicPool calls onRecycle before incrementing the activeCount
		auto index = getPool().activeCount();
		button->getOnToggleEvent().subscribe([this, index, button](ToggleState state)
		{
			if(state == ToggleState::On)
			{
				auto& pool = this->getPool();
				auto buttons = pool.getActives();
				for(auto& otherButton : buttons)
				{
					if(otherButton != button)
						otherButton->setToggleState(ToggleState::Off);
				}
				this->m_onSelectEvent.publish(index);
			}
		});
	}
}