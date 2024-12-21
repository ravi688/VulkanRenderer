#include <sutk/ToggleButton.hpp>
#include <sutk/ToggleButtonGraphic.hpp>
#include <sutk/Constants.hpp> // for SUTK::Constants

namespace SUTK
{
	ToggleButton::ToggleButton(UIDriver& driver, Container* parent, ToggleState state, bool isCreateDefaultGraphic, std::optional<GfxDriverObjectHandleType> textGroup) noexcept : Button(driver, parent, false), m_toggleState(state), m_isOneWayToggle(false), m_toggleGraphic(NULL)
	{
		setSize(Constants::Defaults::ToggleButton::Size);
		if(isCreateDefaultGraphic)
		{
			m_defaultToggleGraphic = driver.createContainer<DefaultToggleButtonGraphic>(this, textGroup);
			setGraphic(m_defaultToggleGraphic);
		}
	}
	ToggleButton::~ToggleButton() noexcept
	{
		if(m_defaultToggleGraphic)
			getUIDriver().destroyContainer<DefaultToggleButtonGraphic>(m_defaultToggleGraphic);
	}
	
	bool ToggleButton::onMouseClick(MouseButton button, KeyEvent action)
	{
		// Mandatory to be called
		Button::onMouseClick(button, action);

		if(button != MouseButton::Left)
			return true;
		if((action == KeyEvent::Press) && (!m_isOneWayToggle || (getToggleState() != ToggleState::On)))
			toggle();
		return true;
	}

	void ToggleButton::clearAllEvents() noexcept
	{
		// Mandatory to be called
		Button::clearAllEvents();

		m_onToggleEvent.clear();
	}

	void ToggleButton::setGraphic(IButtonGraphic* graphic) noexcept
	{
		Button::setGraphic(graphic);
		setToggleGraphic(dynamic_cast<IToggleButtonGraphic*>(graphic));
	}

	void ToggleButton::setToggleGraphic(IToggleButtonGraphic* graphic) noexcept
	{
		m_toggleGraphic = graphic;
		if(graphic != NULL)
			graphic->onToggle(m_toggleState);
	}

	void ToggleButton::toggle() noexcept
	{
		if(getToggleState() == ToggleState::On)
			setToggleState(ToggleState::Off);
		else // if ToggleState::Off
			setToggleState(ToggleState::On);
	}

	void ToggleButton::setToggleState(ToggleState state) noexcept
	{
		if(getToggleState() == state)
			return;
		m_toggleState = state;
		if(m_toggleGraphic != NULL)
			m_toggleGraphic->onToggle(state);
		m_onToggleEvent.publish(state);
	}
}