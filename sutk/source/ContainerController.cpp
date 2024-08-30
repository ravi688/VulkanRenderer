#include <sutk/ContainerController.hpp>

#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/IColorable.hpp>

namespace SUTK
{
	static void RenderablesSetActive(Container* container, bool isActive) noexcept;

	ContainerController::ContainerController(Container* container, bool isActive) noexcept : m_container(container),
																					m_transAutomaton(
																					{ 
																						{ State::Hidden, 0 },
																						{ State::Visible, 1.0f }
																					})
	{
		m_transAutomaton.setTransitionDelay(0.08f);
		m_transAutomaton.getEvent(State::Hidden).subscribe([this](void*) { RenderablesSetActive(m_container, false); });
		if(!isActive)
		{
			_com_assert(m_transAutomaton.getState() == State::Hidden);
			setAlpha(m_transAutomaton.getValue());
			setActive(false);
		}
		else
			m_transAutomaton.setDefault(State::Visible);
	}

	static void RenderablesSetActive(Container* container, bool isActive) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			renderable->setActive(isActive);
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesSetActive(child, isActive);
	}

	static void RenderablesSetAlpha(Container* container, f32 alpha) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			IColorable* colorable = dynamic_cast<IColorable*>(renderable);
			Color4 color = colorable->getColor();
			color.alpha(alpha * 255);
			colorable->setColor(color);
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesSetAlpha(child, alpha);
	}

	void ContainerController::setActive(bool isActive) noexcept
	{
		RenderablesSetActive(m_container, isActive);
	}

	void ContainerController::setAlpha(f32 alpha) noexcept
	{
		RenderablesSetAlpha(m_container, alpha);
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