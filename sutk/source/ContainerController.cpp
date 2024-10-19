#include <sutk/ContainerController.hpp>

#include <sutk/ContainerUtility.hpp>

namespace SUTK
{
	ContainerController::ContainerController(Container* container, bool isActive) noexcept : m_container(container),
																					m_transAutomaton(
																					{ 
																						{ State::Hidden, 0 },
																						{ State::Visible, 1.0f }
																					})
	{
		m_transAutomaton.setTransitionDelay(0.08f);
		m_transAutomaton.getEvent(State::Hidden).subscribe([this]() { this->setActive(com::Bool::False()); });
		if(!isActive)
		{
			// Thos automatically called the handler (above subscribed)
			m_transAutomaton.setState(State::Hidden);
			_com_assert(m_transAutomaton.getState() == State::Hidden);
			setAlpha(m_transAutomaton.getValue());
		}
		else
			m_transAutomaton.setState(State::Visible);
	}

	void ContainerController::setActive(com::Bool isActive) noexcept
	{
		ContainerUtility::SetActiveAllRecursive(m_container, isActive);
	}

	void ContainerController::setAlpha(f32 alpha) noexcept
	{
		ContainerUtility::RenderablesSetAlpha(m_container, alpha);
	}

	bool ContainerController::isRunning()
	{
		return m_transAutomaton.isRunning();
	}

	void ContainerController::update()
	{
		m_transAutomaton.update();
		setAlpha(m_transAutomaton.getValue());
	}

	void ContainerController::present() noexcept
	{
		setActive(com::Bool::True());
		std::cout << "Present" << std::endl;
		m_transAutomaton.transitionTo(State::Visible);
	}

	void ContainerController::hide() noexcept
	{
		m_transAutomaton.transitionTo(State::Hidden);
	}

	void ContainerController::presentImmediate() noexcept
	{
		m_transAutomaton.setState(State::Visible);
	}

	void ContainerController::hideImmediate() noexcept
	{
		m_transAutomaton.setState(State::Hidden);
	}

}