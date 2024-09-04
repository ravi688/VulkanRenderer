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
		m_transAutomaton.getEvent(State::Hidden).subscribe([this](void*) { ContainerUtility::SetActiveAllRecursive(m_container, false); });
		if(!isActive)
		{
			_com_assert(m_transAutomaton.getState() == State::Hidden);
			setAlpha(m_transAutomaton.getValue());
			setActive(false);
		}
		else
			m_transAutomaton.setDefault(State::Visible);
	}

	void ContainerController::setActive(bool isActive) noexcept
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
		setActive(true);
		std::cout << "Present" << std::endl;
		m_transAutomaton.transitionTo(State::Visible);
	}

	void ContainerController::hide() noexcept
	{
		m_transAutomaton.transitionTo(State::Hidden);
	}

}